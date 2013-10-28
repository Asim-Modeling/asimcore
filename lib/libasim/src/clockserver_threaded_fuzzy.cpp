/*
 * **********************************************************************
 *Copyright (C) 2003-2006 Intel Corporation
 *
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */



/**
 * @author Carl Beckmann
 **/

#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <sched.h>

#include "asim/provides/clockserver.h"

#include "asim/clockable.h"
#include "asim/module.h"
#include "asim/smp.h"
#include "asim/rate_matcher.h"


// The global ready time is the time step that the clock server wants the workers to execute.
// The global time events is just a pointer to the clock server's time event list
// that all worker threads can see.
static volatile INT64         GlobalReadyTime = -1;
static deque<CLOCK_REGISTRY> *GlobalTimeEvents = NULL;
static pthread_mutex_t        GlobalTimeEventsLock = PTHREAD_MUTEX_INITIALIZER;
static volatile INT64         CLOCKSERVER_FUZZY_BARRIER_LOOKAHEAD = 0;

#define BEGIN_GLOBAL_TIME_EVENTS_ACCESS pthread_mutex_lock  ( & GlobalTimeEventsLock );
#define   END_GLOBAL_TIME_EVENTS_ACCESS pthread_mutex_unlock( & GlobalTimeEventsLock );


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
    CLOCKSERVER_FUZZY_BARRIER_LOOKAHEAD = LookaheadParam2BaseCycles( threadLookahead );
    list<CLOCK_DOMAIN>::iterator iter_dom = lDomain.begin();
    for ( ; iter_dom != lDomain.end(); ++iter_dom )
    {
        INT64 domain_base_cycles = (*iter_dom)->lClock.front()->nStep;
        VERIFY( CLOCKSERVER_FUZZY_BARRIER_LOOKAHEAD < domain_base_cycles,
                "Fuzzy barrier lookahead exceeds clock period of domain " << (*iter_dom)->name << endl );
    }

    // publish the event list
    GlobalTimeEvents = & lTimeEvents;

    // Create the pthreads if requested
    list<ASIM_CLOCKSERVER_THREAD>::iterator iter_threads = lThreads.begin();
    for( ; iter_threads != lThreads.end(); ++iter_threads)
    {
        (*iter_threads)->CreatePthread();
    }
}


// fuzzy-barrier version of thread work.
// Worker threads continually execute this routine during parallel execution.
// They wait for the clock server to advance the simulation time,
// then they traverse the event list looking for work to do,
// up to the allowed lookahead limit.
void * ASIM_CLOCKSERVER_THREAD_CLASS::ThreadWork(void* param)
{
    ASIM_CLOCKSERVER_THREAD parent = (ASIM_CLOCKSERVER_THREAD)param;
    VERIFYX(parent != NULL);

    parent->threadActive = true;
    
    INT64 localReadyTime  = -1;  // time we are currently processing
    parent->localDoneTime = -1;  // time we are done processing
    INT64 lastTimeFailed  = -1;  // last global ready time at which we found no work to do

    while(1)
    {      
        //
        // Wait until time advances and we can proceed,
        // or until clockserver terminates this thread.
        //
        WORKER_BEGIN_WAIT;
        while( parent->localDoneTime >= GlobalReadyTime + CLOCKSERVER_FUZZY_BARRIER_LOOKAHEAD
               || GlobalReadyTime == lastTimeFailed )
        {
            if( parent->threadForceExit )                 // if thread is being terminated...
            {
                parent->localDoneTime = GlobalReadyTime;  // ...notify clock server that we are done
                WORKER_CANCEL_WAIT;
                pthread_exit(0);                          // ...and exit
            }
            UINT32 retries = CLOCKSERVER_SPINWAIT_YIELD_INTERVAL;
            while ( --retries && parent->localDoneTime >= GlobalReadyTime + CLOCKSERVER_FUZZY_BARRIER_LOOKAHEAD );
            sched_yield();
        }
        WORKER_END_WAIT;
        
        //
        // find all events at the thread local point in time.
        // The current time point for this thread is the time of the earliest event
        // at or after the global ready time and after the local done time.
        //
        deque<CLOCK_REGISTRY> lClockedEvents;     // list of events at current time point
        CLOCK_REGISTRY_EVENTS_ITERATOR it_event;  // for iterating over events lists
        localReadyTime = lastTimeFailed = GlobalReadyTime;
        BEGIN_GLOBAL_TIME_EVENTS_ACCESS
        if ( localReadyTime <= parent->localDoneTime )
        {
            // if looking ahead, and I have already finished the work at the global ready time,
            // find the earliest event after my local done time still within my lookahead window.
            localReadyTime = parent->localDoneTime + 1;
            for ( it_event = GlobalTimeEvents->begin(); it_event != GlobalTimeEvents->end(); ++it_event)
            {
                INT64 eventTime = (*it_event)->nBaseCycle;
                if      ( eventTime >  GlobalReadyTime + CLOCKSERVER_FUZZY_BARRIER_LOOKAHEAD )
                {
                    // past lookahead window, nothing to do!
                    break;
                }
                else if ( eventTime >= localReadyTime )
                {
                    // found one!
                    localReadyTime = eventTime;
                    break;
                }
            }
        }
        for ( it_event = GlobalTimeEvents->begin(); it_event != GlobalTimeEvents->end(); ++it_event)
        {
            INT64 eventTime = (*it_event)->nBaseCycle;
            if      ( eventTime == localReadyTime )
            {
                lClockedEvents.push_back( (*it_event) );
            }
            else if ( eventTime >  localReadyTime )
            {
                break;
            }
        }
        END_GLOBAL_TIME_EVENTS_ACCESS

        UINT32 workDone = 0;

        //
        // for each event at the current time point, run through all the modules
        // that must be clocked for this event, and execute it if it belongs to this thread.
        //
        for( it_event = lClockedEvents.begin(); it_event != lClockedEvents.end(); ++it_event)
        {   
            CLOCK_REGISTRY_MODULES_ITERATOR endM = (*it_event)->lModules.end();
            CLOCK_REGISTRY_MODULES_ITERATOR iter = (*it_event)->lModules.begin();
            for( ; iter != endM; ++iter)
            {
                if ( parent == (*iter).first->GetClockingThread() ) {
                    (*iter).second->currentCycle = (*it_event)->nCycle;
                    (*iter).second->Clock();
                    workDone++;
                }
            }
        }

        //
        // Do the same for the write rate matcher ports, execute them if they belong to me.
        //
        for( it_event = lClockedEvents.begin(); it_event != lClockedEvents.end(); ++it_event)
        {   
            CLOCK_REGISTRY_MODULES_ITERATOR endM = (*it_event)->lWriterRM.end();
            CLOCK_REGISTRY_MODULES_ITERATOR iter = (*it_event)->lWriterRM.begin();
            for( ; iter != endM; ++iter)
            {
                // !!$#@!!! RATE_MATCHER and ASIM_CLOCKABLE both have different
                // GetClockingThread() routines, that do not inherit from one another.
                // We need to do this cast here to make sure we call the RATE_MATCHER one
                RATE_MATCHER wrm = (RATE_MATCHER)(*iter).first;
                if ( parent == wrm->GetClockingThread() ) {
                    (*iter).second->currentCycle = (*it_event)->nCycle;
                    (*iter).second->Clock();
                    workDone++;
                }
            }
        }
        
        // the following keeps eager worker threads from starving the clock server thread
        // when there is no work for them to do:
        if ( workDone > 0 )
        {
            lastTimeFailed = -1;
        }

        //
        // now advance my local time
        //
        parent->localDoneTime = localReadyTime;
        WORKER_SEND_SIGNAL;
    }
    return 0;
}


// fuzzy barrier version of threaded clock routine.
// The system calls this for each clock tick.
// The clock server advances simulation time, allowing the worker threads to proceed.
// It then waits for the worker threads to complete at least up to the given time,
// then returns to the caller.
UINT64 ASIM_CLOCK_SERVER_CLASS::ThreadedClock() 
{
    //
    // advance simulation time to the timepoint at the head of the events list.
    // This will allow worker threads to proceed forward.
    //
    INT64 currentBaseCycle = lTimeEvents.front()->nBaseCycle;
    GlobalReadyTime = currentBaseCycle;             // signal the worker threads!
    SERVER_SEND_SIGNAL;

    //
    // spin-wait for all worker threads to finish the current time point
    //
    SERVER_BEGIN_WAIT;
    while ( GetGlobalDoneTime() < currentBaseCycle )
    {
        UINT32 retries = CLOCKSERVER_SPINWAIT_YIELD_INTERVAL;
        while ( --retries && GetGlobalDoneTime() < currentBaseCycle );
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
    BEGIN_GLOBAL_TIME_EVENTS_ACCESS
    while ( currentBaseCycle == (INT64)lTimeEvents.front()->nBaseCycle )
    {
        // remove event from head of list at current time
        CLOCK_REGISTRY currentEvent = lTimeEvents.front();
        lTimeEvents.pop_front();
        // update cycle count and time
        currentEvent->nCycle++;
        currentEvent->nBaseCycle += currentEvent->nStep;
        // re-add to list at next time
        AddTimeEvent(currentEvent->nBaseCycle, currentEvent);
    }
    END_GLOBAL_TIME_EVENTS_ACCESS

    // FIX FIX FIX !?! generate DRAL new cycle event if necessary.

    //
    // Return the number of base cycles forwarded                       
    //
    UINT64 currentBaseCycleMod = currentBaseCycle/100;
    UINT64 inc = currentBaseCycleMod - internalBaseCycle;  
    internalBaseCycle = currentBaseCycleMod; 
    return inc;
}
