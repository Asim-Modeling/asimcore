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
 * @author Santi Galan, Ken Barr, Ramon Matas Navarro, Carl Beckmann
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


#if CLOCKSERVER_SINGLE_WORKER_SIGNAL==1
// the clock server main thread tells the workers to start executing
// by setting BarrierCount to the number of threas, then flipping BarrierCurPhase.
// The workers execute when this becomes not equal to the barrierPhase member
// variable in ASIM_CLOCKSERVER_THREAD_CLASS.  When workers arrive at the
// barrier, they decrement BarrierCount, and when it reaches zero, the
// main thread restarts the counter and flips BarrierCurPhase to start all over again.
static volatile bool  BarrierCurPhase = false;
static ATOMIC32_CLASS BarrierCount    = 0;
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

    // Create the pthreads if requested
    list<ASIM_CLOCKSERVER_THREAD>::iterator iter_threads = lThreads.begin();
    for( ; iter_threads != lThreads.end(); ++iter_threads)
    {
        (*iter_threads)->CreatePthread();
    }
}


//
// worker threads continually execute this routine during parallel execution.
// They wait for a signal from the clock server, then execute all the tasks
// on their task queue, then signal the clock server back when complete.
//
void * ASIM_CLOCKSERVER_THREAD_CLASS::ThreadWork(void* param)
{
#if CLOCKSERVER_SINGLE_WORKER_SIGNAL==1
# define SPIN_WAIT_CONDITION  BarrierCurPhase == parent->barrierPhase &&     \
                              ! parent->threadForceExit
# define NOTIFY_DONE          parent->tasks_completed = true;                \
                              parent->barrierPhase = ! parent->barrierPhase; \
                              --BarrierCount
#else
# define SPIN_WAIT_CONDITION  parent->tasks_completed
# define NOTIFY_DONE          parent->tasks_completed = true
#endif
    
    ASIM_CLOCKSERVER_THREAD parent = (ASIM_CLOCKSERVER_THREAD)param;
    VERIFYX(parent != NULL);

    parent->threadActive = true;
    
    // Before enter the loop lock the task_list_mutex to avoid any race condition
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
    pthread_mutex_lock(&(parent->task_list_mutex));
#endif
    while(1)
    {      
        // IMPORTANT!! We don't need to get/release the lock in this loop
        // since pthread_cond_wait does it for us.

        // Wait until we get some work to perform
        while( SPIN_WAIT_CONDITION )
        {
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
            pthread_cond_wait(&(parent->wait_condition), &(parent->task_list_mutex));
#else
            UINT32 retries = CLOCKSERVER_SPINWAIT_YIELD_INTERVAL;
            while ( --retries && SPIN_WAIT_CONDITION );
            sched_yield();
#endif
        }

        // Check if we have to exit...
        if(parent->threadForceExit)
        {
            // Not needed, for sanity...
            NOTIFY_DONE;
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
            // Make sure to release the lock before exiting!
            pthread_mutex_unlock(&(parent->task_list_mutex));
#endif
            pthread_exit(0);
        }
        
        CLOCK_CALLBACK_INTERFACE cb = parent->GetNextWorkItem();
        while(cb != NULL)
        {              
            cb->Clock();
            cb = parent->GetNextWorkItem();
        }

        // Tell the master thread that all the work is done
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
        pthread_mutex_lock(&(parent->finish_cond_mutex));
#endif
        NOTIFY_DONE;
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
        pthread_cond_broadcast(&(parent->finish_condition));
        pthread_mutex_unlock(&(parent->finish_cond_mutex));
#endif
                
    }    
    
    return 0;
#undef SPIN_WAIT_CONDITION
#undef NOTIFY_DONE
}


//
// the following two routines are used by the clock server
// during parallel execution to signal and then wait for
// the worker threads.
//
/** Returns true if all the assigned tasks have been completed */
bool ASIM_CLOCKSERVER_THREAD_CLASS::CheckAllTasksCompleted()
{
    ASSERTX(ThreadActive());

    // Wait for the thread to complete the tasks
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
    pthread_mutex_lock(&finish_cond_mutex);
#endif
    while(!tasks_completed)
    {
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
        pthread_cond_wait(&finish_condition, &finish_cond_mutex);
#else
        UINT32 retries = CLOCKSERVER_SPINWAIT_YIELD_INTERVAL;
        while ( --retries && !tasks_completed );
        sched_yield();
#endif
    }
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
    pthread_mutex_unlock(&finish_cond_mutex);
#endif

    return tasks_completed;
}

//
// start a worker thread executing all its task,
// but do not wait for it to finish, just return immediately.
//
void ASIM_CLOCKSERVER_THREAD_CLASS::PerformTasksThreaded()
{
    ASSERTX(ThreadActive());

    // don't bother waking the thread up if there's no work to do
    if ( tasks.empty() ) {
      tasks_completed = true;
      return;
    }

    // Restart the thread waiting for the condition
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
    pthread_mutex_lock(&task_list_mutex);
#endif
    tasks_completed = false;
#if CLOCKSERVER_USES_PTHREADS_SIGNALLING==1
    pthread_cond_broadcast(&wait_condition);
    pthread_mutex_unlock(&task_list_mutex);
#endif
}


//
// Threaded clock server.
// The server runs in the main thread, but all the model work
// happens in other threads, executing in the ThreadWork() routine.
// This version of the threaded clock server barrier synchronizes every clock cycle.
//
UINT64 ASIM_CLOCK_SERVER_CLASS::ThreadedClock() 
{
    deque<CLOCK_REGISTRY>          lClockedEvents;  // list of events actually processed here
    CLOCK_REGISTRY_EVENTS_ITERATOR it_event;        // for iterating over events lists
    CLOCKSERVER_THREADS_ITERATOR   iter_threads;    // for iterating over all worker threads

    //
    // run through the list of events and find all events at the current point in time
    //
    UINT64  currentBaseCycle =  lTimeEvents.front()->nBaseCycle;
    while ( currentBaseCycle == lTimeEvents.front()->nBaseCycle )
    {
        CLOCK_REGISTRY currentEvent = lTimeEvents.front();
        lTimeEvents.pop_front();
        lClockedEvents.push_back( currentEvent );
    }
    
    //
    // for each event at the current time point, run through all the modules
    // that must be clocked for this event, and assign the task to its worker thread.
    //
    for( it_event = lClockedEvents.begin(); it_event != lClockedEvents.end(); ++it_event)
    {   
        CLOCK_REGISTRY_MODULES_ITERATOR endM = (*it_event)->lModules.end();
        CLOCK_REGISTRY_MODULES_ITERATOR iter = (*it_event)->lModules.begin();
        for( ; iter != endM; ++iter)
        {
            (*iter).second->currentCycle = (*it_event)->nCycle;
            (*iter).first->GetClockingThread()->AssignTask((*iter).second);
        }
    }
     
    //
    // Do the same for the write rate matcher ports, assign them to the worker threads.
    // This relies on the fact that AssignTask queues tasks in the order that they get
    // executed, since the write rate matchers must be clocked AFTER the module clock calls.
    //
    for( it_event = lClockedEvents.begin(); it_event != lClockedEvents.end(); ++it_event)
    {   
        CLOCK_REGISTRY_MODULES_ITERATOR endM = (*it_event)->lWriterRM.end();
        CLOCK_REGISTRY_MODULES_ITERATOR iter = (*it_event)->lWriterRM.begin();
        for( ; iter != endM; ++iter)
        {
            (*iter).second->currentCycle = (*it_event)->nCycle;
            // !!$#@!!! RATE_MATCHER and ASIM_CLOCKABLE both have different
            // GetClockingThread() routines, that do not inherit from one another.
            // We need to do this cast here to make sure we call the RATE_MATCHER one
            RATE_MATCHER wrm = (RATE_MATCHER)(*iter).first;
            wrm->GetClockingThread()->AssignTask((*iter).second);
        }
    }
  
    //
    // wake up all the worker threads
    //
    StartAllWorkerThreads();

#if CLOCKSERVER_READDS_EVENTS_CONCURRENTLY==0
    //
    // barrier-synchronize on all the worker threads to finish
    //
    WaitForAllWorkerThreads();
#endif

    //
    // for each event that we processed,
    // re-add it to the events list at its next time step.
    // This has to be done after the worker threads have finished, since
    // step may have been modified by setDomainFrequency during the clocking.
    //
    for( it_event = lClockedEvents.begin(); it_event != lClockedEvents.end(); ++it_event)
    {
        (*it_event)->nCycle++;

        (*it_event)->nBaseCycle += (*it_event)->nStep;
        AddTimeEvent((*it_event)->nBaseCycle, (*it_event));
    }

#if CLOCKSERVER_READDS_EVENTS_CONCURRENTLY==1
    //
    // barrier-synchronize on all the worker threads to finish
    //
    WaitForAllWorkerThreads();
#endif

    // FIX FIX FIX !?! generate DRAL new cycle event if necessary.

    //
    // Return the number of base cycles forwarded                       
    //
    UINT64 currentBaseCycleMod = currentBaseCycle/100;
    UINT64 inc = currentBaseCycleMod - internalBaseCycle;  
    internalBaseCycle = currentBaseCycleMod; 
    return inc;
}


//
// start all worker threads running
//
void ASIM_CLOCK_SERVER_CLASS::StartAllWorkerThreads()
{
#if CLOCKSERVER_SINGLE_WORKER_SIGNAL==1
    ///
    /// NEW VERSION.  USES A SINGLE SHARED BARRIER VARIABLE
    ///
    BarrierCount    = lThreads.size();
    MemBarrier();     // enforce update order of count and phase
    BarrierCurPhase = ! BarrierCurPhase;

#else
    ///
    /// OLD VERSION.  USES INDIVIDUAL FLAGS FOR EACH THREAD
    ///
    CLOCKSERVER_THREADS_ITERATOR   iter_threads;    // for iterating over all worker threads

#if CLOCKSERVER_THREAD_RUNS_FIRST_TASK==1
    ASIM_CLOCKSERVER_THREAD first_one = NULL;
#endif
    for( iter_threads = lThreads.begin(); iter_threads != lThreads.end(); ++iter_threads)
    {
#if CLOCKSERVER_THREAD_RUNS_FIRST_TASK==1
        if ( first_one ) {
#endif
            (*iter_threads)->PerformTasksThreaded();
#if CLOCKSERVER_THREAD_RUNS_FIRST_TASK==1
        } else {
            first_one = (*iter_threads);
        }
#endif
    }
#if CLOCKSERVER_THREAD_RUNS_FIRST_TASK==1
    first_one->PerformTasksSequential();
#endif

#endif
}

//
// wait for all worker threads to complete
//
void ASIM_CLOCK_SERVER_CLASS::WaitForAllWorkerThreads()
{
#if CLOCKSERVER_SINGLE_WORKER_SIGNAL==1
    ///
    /// NEW VERSION.  USES A SINGLE SHARED BARRIER VARIABLE
    ///
    while ( int(BarrierCount) > 0 ) {
        UINT32 retries = CLOCKSERVER_SPINWAIT_YIELD_INTERVAL;
        while ( --retries && int(BarrierCount) > 0 );
        sched_yield();
    }

#else
    ///
    /// OLD VERSION.  USES INDIVIDUAL FLAGS FOR EACH THREAD
    ///
    CLOCKSERVER_THREADS_ITERATOR   iter_threads;    // for iterating over all worker threads

    for( iter_threads = lThreads.begin(); iter_threads != lThreads.end();               )
    {
        if((*iter_threads)->CheckAllTasksCompleted())
            ++iter_threads;
    }

#endif
}
