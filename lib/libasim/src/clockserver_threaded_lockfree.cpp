/*
 * **********************************************************************
 * Copyright (c) 2014, Intel Corporation
 *
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright 
 *   notice, this list of conditions and the following disclaimer in the 
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the Intel Corporation nor the names of its 
 *   contributors may be used to endorse or promote products derived from 
 *   this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



/**
 * @author Carl Beckmann
 **/

#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <sched.h>
#include <string>

// If compiling the clockserver into libasim, get the header files from the
// libasim common area, and hardwire some necessary param values...
#ifdef CLOCKSERVER_IN_LIBASIM
# define CLOCKSERVER_SPINWAIT_YIELD_INTERVAL 500
# define CLOCKSERVER_INSTRUMENT_TPROFILER    0
# include "asim/clockserver.h"
# include "asim/time_events_ring.h"

// ... but if compiling as a module, get header files from AWB-provided area,
// which will also supply the params.  (This is a bit of a hack, admittedly)
#else
# include "asim/provides/clockserver.h"
# include "asim/restricted/time_events_ring.h"
#endif

#include "asim/clockable.h"
#include "asim/module.h"
#include "asim/rate_matcher.h"

#if CLOCKSERVER_INSTRUMENT_TPROFILER==1

#include "libittnotify.h"
// these bogus locations are needed by the Thread profiler,
// to differentiate between two synchronization instances:
UINT64 server2worker_bogus_sync = 0;
UINT64 worker2server_bogus_sync = 0;
#define WORKER_BEGIN_WAIT  __itt_notify_sync_prepare(   (void *)server2worker_bogus_sync )
#define WORKER_CANCEL_WAIT __itt_notify_sync_cancel(    (void *)server2worker_bogus_sync )
#define WORKER_END_WAIT    __itt_notify_sync_acquired(  (void *)server2worker_bogus_sync )
#define WORKER_SEND_SIGNAL __itt_notify_sync_releasing( (void *)worker2server_bogus_sync )
#define SERVER_BEGIN_WAIT  __itt_notify_sync_prepare(   (void *)worker2server_bogus_sync )
#define SERVER_END_WAIT    __itt_notify_sync_acquired(  (void *)worker2server_bogus_sync )
#define SERVER_SEND_SIGNAL __itt_notify_sync_releasing( (void *)server2worker_bogus_sync )

#else

#define WORKER_BEGIN_WAIT
#define WORKER_END_WAIT
#define WORKER_CANCEL_WAIT
#define WORKER_SEND_SIGNAL
#define SERVER_BEGIN_WAIT
#define SERVER_END_WAIT
#define SERVER_SEND_SIGNAL

#endif


// there is one global time-events list
TIME_EVENTS_RING_CLASS GlobalTimeRing;


//
// the required factory method to create clockserver thread objects.
//
ASIM_CLOCKSERVER_THREAD new_ASIM_CLOCKSERVER_THREAD_CLASS( ASIM_SMP_THREAD_HANDLE th )
{
    return new ASIM_CLOCKSERVER_THREAD_CLASS( th );
}


//
// Initialize this threaded clockserver implementation
//
void ASIM_CLOCK_SERVER_CLASS::InitClockServerThreaded()
{
    UINT32 max_pthreads = ASIM_SMP_CLASS::GetMaxThreads();
    VERIFY(lThreads.size() <= max_pthreads, "Max pthreads set to "
            << max_pthreads << " and the model is trying to create "
            << lThreads.size() << " pthreads");

    // set the fuzy barrier lookahead
    GlobalTimeRing.set_lookahead( LookaheadParam2BaseCycles( threadLookahead ) );
    
    // add the events to the lock-free event list
    CLOCK_REGISTRY_EVENTS_ITERATOR iter_ev = lTimeEvents.begin();
    for ( ; iter_ev != lTimeEvents.end(); ++iter_ev )
    {
        GlobalTimeRing.insert( *iter_ev );
    }

    // Create the pthreads if requested
    list<ASIM_CLOCKSERVER_THREAD>::iterator iter_threads = lThreads.begin();
    for( ; iter_threads != lThreads.end(); ++iter_threads)
    {
        (*iter_threads)->CreatePthread();
    }
}


//
// fuzzy barrier thread work routine, with lock-free event list
//
void * ASIM_CLOCKSERVER_THREAD_CLASS::ThreadWork(void* param)
{
    ASIM_CLOCKSERVER_THREAD parent = (ASIM_CLOCKSERVER_THREAD)param;
    VERIFYX(parent != NULL);

    parent->threadActive   = true;          // this thread is active
    parent->localDoneTime  = -1;            // we have not finished any work yet
    TIME_EVENTS_RING_CLASS::ITERATOR
        myNextEvent( &GlobalTimeRing );     // start a persistent index into event list
   
    while(1)
    {      
        //
        // Wait until time advances and we can proceed,
        // or until clockserver terminates this thread.
        //
        WORKER_BEGIN_WAIT;
        while( ! myNextEvent.is_ready() )
        {
            if( parent->threadForceExit )                     // if thread is being terminated...
            {
                parent->localDoneTime = GlobalTimeRing.front()->GetBaseCycle();  // notify server we're done,
                WORKER_CANCEL_WAIT;
                pthread_exit(0);                                                 // and exit.
            }
            UINT32 retries = CLOCKSERVER_SPINWAIT_YIELD_INTERVAL;
            while ( ! myNextEvent.is_ready() && --retries ) ;
            sched_yield();
        }
        WORKER_END_WAIT;
        
        //
        // find all events at the thread local point in time,
        // and advance my index into the global events list.
        //
        deque<TIME_EVENT_INSTANCE> lClockedEvents;            // list of events at current time point
        INT64 localReadyTime = myNextEvent.time();
        while ( myNextEvent.is_ready() && myNextEvent.time() == localReadyTime )
        {
            lClockedEvents.push_back( *myNextEvent );
            ++myNextEvent;
        }

        //
        // for each event at the current time point, run through all the modules
        // that must be clocked for this event, and execute it if it belongs to this thread.
        //
        TIME_EVENT_INSTANCE_ITERATOR it_event;
        for( it_event = lClockedEvents.begin(); it_event != lClockedEvents.end(); ++it_event )
        {   
            CLOCK_REGISTRY_MODULES_ITERATOR endM = (*it_event)->GetModuleList()->end();
            CLOCK_REGISTRY_MODULES_ITERATOR iter = (*it_event)->GetModuleList()->begin();
            for( ; iter != endM; ++iter)
            {
                if ( parent == (*iter).first->GetClockingThread() ) {
                    (*iter).second->currentCycle = (*it_event)->GetCycle();
                    (*iter).second->Clock();
                }
            }
        }

        //
        // Do the same for the write rate matcher ports, execute them if they belong to me.
        //
        for( it_event = lClockedEvents.begin(); it_event != lClockedEvents.end(); ++it_event )
        {   
            CLOCK_REGISTRY_MODULES_ITERATOR endM = (*it_event)->GetWriterRMList()->end();
            CLOCK_REGISTRY_MODULES_ITERATOR iter = (*it_event)->GetWriterRMList()->begin();
            for( ; iter != endM; ++iter)
            {
                // !!$#@!!! RATE_MATCHER and ASIM_CLOCKABLE both have different
                // GetClockingThread() routines, that do not inherit from one another.
                // We need to do this cast here to make sure we call the RATE_MATCHER one
                RATE_MATCHER wrm = (RATE_MATCHER)(*iter).first;
                if ( parent == wrm->GetClockingThread() ) {
                    (*iter).second->currentCycle = (*it_event)->GetCycle();
                    (*iter).second->Clock();
                }
            }
        }

        //
        // now advance my local time.
        // This is a synchronization update, since it tells the
        // clock server thread that time can advance.
        //
        parent->localDoneTime = localReadyTime;
        WORKER_SEND_SIGNAL;
    }

    return 0;   // should never get here.  We normally exit from the threadForceExit check in the loop.
}


//
// Fuzzy barrier threaded clock routine, with lock-free event list.
// This basically just advances the ready time, waits for worker threads to catch up,
// updates the event list, and then returns.
//
UINT64 ASIM_CLOCK_SERVER_CLASS::ThreadedClock() 
{
    //
    // advance simulation time to the timepoint at the head of the events list.
    // This will allow worker threads to proceed forward.
    //
    INT64 currentBaseCycle = GlobalTimeRing.advance_time();
    SERVER_SEND_SIGNAL;

#define WAIT_CONDITION   GetGlobalDoneTime() < currentBaseCycle

    //
    // spin-wait for all worker threads to finish the current time point
    //
    SERVER_BEGIN_WAIT;
    while ( WAIT_CONDITION )
    {
        UINT32 retries = CLOCKSERVER_SPINWAIT_YIELD_INTERVAL;
        while ( --retries && WAIT_CONDITION ) ;
        sched_yield();
    }
    SERVER_END_WAIT;

    //
    // remove all events at the current time point from the front of the list,
    // and re-add them to the events list at their next time step.
    // This has to be done after the worker threads have finished, since
    // the workers traverse the events list themselves, and also since the
    // step may have been modified by setDomainFrequency during the clocking.
    //
    while ( currentBaseCycle == (INT64)GlobalTimeRing.front()->GetBaseCycle() )
    {
        TIME_EVENT_INSTANCE currentEvent = GlobalTimeRing.pop_front();
        currentEvent->AdvanceCycle();
        GlobalTimeRing.insert( currentEvent );
    }

    // FIX FIX FIX !?! generate DRAL new cycle event if necessary.

    //
    // Return the number of base cycles forwarded                       
    //
    UINT64 currentBaseCycleMod = currentBaseCycle/100;
    UINT64 inc = currentBaseCycleMod - internalBaseCycle;  
    internalBaseCycle = currentBaseCycleMod; 
    return inc;
}
