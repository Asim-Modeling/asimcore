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
 *
 * @desc   Lock-free fuzzy barrier with dynamic thread switching
 **/

/////////////////////////////////////////////////////////////////////////
//
// INCLUDE FILES, MACROS, PARAMETERS, AND CONSTANTS
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <sched.h>
#include <string>
#include <pthread.h>

// If compiling the clockserver into libasim, get the header files from the
// libasim common area, and hardwire some necessary param values...
#ifdef CLOCKSERVER_IN_LIBASIM
# define CLOCKSERVER_SPINWAIT_YIELD_INTERVAL 2
# define CLOCKSERVER_MAX_WORKER_PTHREADS     7
# define CLOCKSERVER_THREAD_IS_WORKER        0
# define CLOCKSERVER_SCHEDULING_ALGORITHM    "Simple"
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
#define SERVER_CANCEL_WAIT __itt_notify_sync_cancel(    (void *)worker2server_bogus_sync )
#define SERVER_END_WAIT    __itt_notify_sync_acquired(  (void *)worker2server_bogus_sync )
#define SERVER_SEND_SIGNAL __itt_notify_sync_releasing( (void *)server2worker_bogus_sync )

#else

#define WORKER_BEGIN_WAIT
#define WORKER_END_WAIT
#define WORKER_CANCEL_WAIT
#define WORKER_SEND_SIGNAL
#define SERVER_BEGIN_WAIT
#define SERVER_CANCEL_WAIT
#define SERVER_END_WAIT
#define SERVER_SEND_SIGNAL

#endif


/////////////////////////////////////////////////////////////////////////
//
// TYPEDEFS AND CLASSES
//
/////////////////////////////////////////////////////////////////////////

//
// A "dynamic task" is just a "clockserver thread" object,
// with a few additional fields:
//  -- a mutex, locked by the pthread that is currently running this task
//  -- a persistent index into the time events list
//
// There is also a global list of all dynamic tasks allocated.
//
typedef
class DYNAMIC_TASK_CLASS * DYNAMIC_TASK;
class DYNAMIC_TASK_CLASS : public ASIM_CLOCKSERVER_THREAD_CLASS
{
  private:
    pthread_mutex_t                  mutex;               // a lock so only one pthread owns it
    TIME_EVENTS_RING_CLASS::ITERATOR next_event;          // a persistent index into event list
    static INT32                     NumPthreads;         // global count of all pthreads created and not yet destroyed
    friend ASIM_CLOCKSERVER_THREAD
       new_ASIM_CLOCKSERVER_THREAD_CLASS
                      ( ASIM_SMP_THREAD_HANDLE th );      // public factory method
    DYNAMIC_TASK_CLASS( ASIM_SMP_THREAD_HANDLE th );      // private constructor

  public:
    // the following functions are not virtual, and are safe to be called
    // when this == NULL.  This makes it easier to code certain algorithms
    // that use them, because we don't have to repeat the this==NULL check.
    // In the case of the time() method, which is used to calculate minimum
    // times, we return a big number when this==NULL:
    bool   try_lock();                                    // try to acquire this task but do not block
    void   assign_to_this_pthread();                      // set ownership of task to the current pthread
    void   release_from_pthread();                        // release ownership of task from current pthread
    void   unlock();                                      // release this task and clear its "active" flag
    bool   is_ready() { return this != NULL &&
                               next_event.is_ready();  }; // is this task ready to execute?
    bool   is_done()  { return this != NULL &&
                               threadForceExit;        }; // is this task being forced to exit?
    INT64  time()     { return this == NULL ? INT64_MAX :
                               next_event.time();      }; // time (base cycles) of next event

    TIME_EVENT_INSTANCE
           get_next_event()   { return *next_event;    }; // return a pointer to next event
    void   advance_event()    {       ++next_event;    }; // increment the index into the time events list
    bool   next_event_valid() { next_event.is_valid(); }; // is the next event pointing at a valid entry?

    void         DoWork();                                // execute one cycle of work in this task
    static void  MainThreadTaskRelease( DYNAMIC_TASK& );  // relinquish work task and return to main thread
    virtual void CreatePthread( bool active = true );     // create a thread.  Replaces similar base class function,
                                                          // but a pthread will only be created if the flag is true
    virtual bool DestroyPthread();                        // destroy the pthread.  Virtual since called by base class
    void *       ExitPthread();                           // end the pthread.  Called from the running pthread
};

//
// the dynamic schedule class provides the TaskSwitch() routine
// to switch to the next runnable task.
// Derived classes implement different scheduling algorithms.
//
class DYNAMIC_SCHEDULER_CLASS
{
  protected:
    static ATOMIC_INT32 seed;                             // round-robin starting point for task list search
  public:
    static vector<DYNAMIC_TASK> AllTasks;                 // global list of all dynamic tasks
    static DYNAMIC_SCHEDULER_CLASS *
        new_scheduler( string, bool = false );            // factory method to create new scheduler of given type
    virtual void TaskSwitch( DYNAMIC_TASK& );             // relinquish current task and acquire a new one
    virtual void MainThreadTaskRelease( DYNAMIC_TASK& );  // release task to the scheduler (by main thread)
    virtual void NotifyExiting( DYNAMIC_TASK ) {};        // notify the scheduler this task is being killed
};

class DYNAMIC_SCHEDULER_RR_CLASS : public DYNAMIC_SCHEDULER_CLASS
{
  public:
    virtual void TaskSwitch( DYNAMIC_TASK& );             // READY-TO-RUN scheduling algorithm
};
class DYNAMIC_SCHEDULER_RE_CLASS : public DYNAMIC_SCHEDULER_CLASS
{
  public:
    virtual void TaskSwitch( DYNAMIC_TASK& );             // READY-OR-EARLIEST scheduling algorithm
};
class DYNAMIC_SCHEDULER_AE_CLASS : public DYNAMIC_SCHEDULER_CLASS
{
  public:
    virtual void TaskSwitch( DYNAMIC_TASK& );             // ALWAYS-EARLIEST scheduling algorithm
    virtual void MainThreadTaskRelease( DYNAMIC_TASK& );      // release task to the scheduler
    virtual void NotifyExiting( DYNAMIC_TASK );               // notify scheduler task is being killed
    DYNAMIC_SCHEDULER_AE_CLASS();                             // constructor
    virtual ~DYNAMIC_SCHEDULER_AE_CLASS() {};                 // destructor
  protected:
    static deque<DYNAMIC_TASK> taskq;                         // time ordered task queue
    static pthread_mutex_t     mutex;                         // a lock so only one pthread can modify it
    static DYNAMIC_TASK        exiting;                       // the current task being killed
    static ATOMIC_INT64        nextime;                       // time of task at head of the queue
    //
    void                       qins( DYNAMIC_TASK );          // insert something into the queue
    DYNAMIC_TASK               qrem(              );          // remove the next item from the queue
    void lock()   { pthread_mutex_lock  ( &mutex ); };        // get exclusive access to the queue
    void unlock() { pthread_mutex_unlock( &mutex ); };        // end exclusive access critial section
};
class DYNAMIC_SCHEDULER_AE_MASTER_CLASS : public DYNAMIC_SCHEDULER_AE_CLASS
{
  public:
    virtual void TaskSwitch( DYNAMIC_TASK& );             // ALWAYS-EARLIEST scheduling algorithm (master)
    virtual ~DYNAMIC_SCHEDULER_AE_MASTER_CLASS() {};          // destructor
};

class NULL_MASTER_SCHEDULER_CLASS : public DYNAMIC_SCHEDULER_CLASS
{
  public:
    virtual void TaskSwitch( DYNAMIC_TASK& );             // NULL scheduler (for use if master is not a worker)
};


/////////////////////////////////////////////////////////////////////////
//
// GLOBAL VARIABLES
//
/////////////////////////////////////////////////////////////////////////

// the global dynamic task list, and a
// round-robin starting point for task list search:
vector<DYNAMIC_TASK>     DYNAMIC_SCHEDULER_CLASS::AllTasks;
ATOMIC_INT32             DYNAMIC_SCHEDULER_CLASS::seed       = 0;
// global count of all work pthreads created
INT32                    DYNAMIC_TASK_CLASS::NumPthreads     = 0;

// a time-ordered task queue for the AE scheduler,
// and a lock to guarantee thread-safe access to it:
deque<DYNAMIC_TASK>      DYNAMIC_SCHEDULER_AE_CLASS::taskq;
pthread_mutex_t          DYNAMIC_SCHEDULER_AE_CLASS::mutex   = PTHREAD_MUTEX_INITIALIZER;
// the current task being killed
DYNAMIC_TASK             DYNAMIC_SCHEDULER_AE_CLASS::exiting = NULL;
// time of task at head of the queue
ATOMIC_INT64             DYNAMIC_SCHEDULER_AE_CLASS::nextime = -1;


// there is one global time-events list
TIME_EVENTS_RING_CLASS   GlobalTimeRing;

// the scheduler objects for the main and worker threads
DYNAMIC_SCHEDULER_CLASS *MasterScheduler                     = NULL;
DYNAMIC_SCHEDULER_CLASS *WorkerScheduler                     = NULL;


/////////////////////////////////////////////////////////////////////////
//
// FUNCTIONS AND SUBROUTINES
//
/////////////////////////////////////////////////////////////////////////

//
// the factory method to create clockserver thread objects,
// and the constructor for the dynamic scheduling derived class.
// The constructor initializes the time events list index
// and the mutex, and adds itself to the global list of tasks.
//
ASIM_CLOCKSERVER_THREAD new_ASIM_CLOCKSERVER_THREAD_CLASS( ASIM_SMP_THREAD_HANDLE th )
{
    return new DYNAMIC_TASK_CLASS( th );
}

DYNAMIC_TASK_CLASS::DYNAMIC_TASK_CLASS( ASIM_SMP_THREAD_HANDLE th )
  : ASIM_CLOCKSERVER_THREAD_CLASS( th ),
    next_event( & GlobalTimeRing )
{
    localDoneTime = -1;                 // we have not finished any work yet
    pthread_mutex_init( &mutex, NULL );
    DYNAMIC_SCHEDULER_CLASS::AllTasks.push_back( this );
}


//
// Acquire ownership of a task.
// Call this after successfully acquiring the task's lock.
// Sets the thread-active flag, and the owning thread,
// and makes a call into ASIM_SMP_CLASS to switch the current thread handle.
//
void DYNAMIC_TASK_CLASS::assign_to_this_pthread()
{
    if ( this == NULL ) return;
    thread = pthread_self();
    ASIM_SMP_CLASS::SetThreadHandle( tHandle );
    MemBarrier();        // ensure that "thread" pointer is valid when active flag set
    threadActive = true;
}

//
// try to acquire this task but do not block.
// Returns 1 on successfully acquiring the lock, 0 if the lock was owned by someone else.
//
bool DYNAMIC_TASK_CLASS::try_lock()
{
    if ( this == NULL ) return false;
    return ( EBUSY != pthread_mutex_trylock( &mutex ) );
}

//
// relinquish this task.
// Also clears the thread-active flag.
//
void DYNAMIC_TASK_CLASS::release_from_pthread()
{
    if ( this == NULL ) return;
    threadActive = false;
}

//
// unlock and relinquish this task.
// Also clears the thread-active flag.
//
void DYNAMIC_TASK_CLASS::unlock()
{
    if ( this == NULL ) return;
    threadActive = false;
    pthread_mutex_unlock( &mutex );
}


//
// relinquish the current task (if any), and fetch a new one.
// To avoid starvation, we need to guarantee that all tasks
// will eventually be switched into, so we use a running
// counter as a seed index into the task array.
//
// SIMPLE algorithm: just find the next available task
//
void DYNAMIC_SCHEDULER_CLASS::TaskSwitch( DYNAMIC_TASK &task )
{
    UINT32 offset = seed++;
    UINT32 N = AllTasks.size();
    for ( UINT32 i=0; i<N; i++ )
    {
        UINT32 t = ( i + offset ) % N;
        if ( AllTasks[t] != task     &&
             AllTasks[t]->try_lock()    )
        {
            task->unlock();
            task = AllTasks[t];
            task->assign_to_this_pthread();
            return;
        }
    }
}

//
// READY-TO-RUN algorithm: find the next ready-to-run task.
// If no other task is ready to run, just stick with the current task.
//
void DYNAMIC_SCHEDULER_RR_CLASS::TaskSwitch( DYNAMIC_TASK &task )
{
    UINT32 offset = seed++;
    UINT32 N = AllTasks.size();
    for ( UINT32 i=0; i<N; i++ )
    {
        UINT32 t = ( i + offset ) % N;
        if ( AllTasks[t] != task     &&
             AllTasks[t]->try_lock()    )
        {
            if ( AllTasks[t]->is_ready() || AllTasks[t]->is_done() )
            {
                task->unlock();
                task = AllTasks[t];
                task->assign_to_this_pthread();
                return;
            }
            else
            {
                // task not ready?  release it and keep looking...
                AllTasks[t]->unlock();
            }
        }
    }
}

//
// READY-OR-EARLIEST algorithm: find the next ready-to-run task.
// If no other task is ready to run, switch to the one with the
// earliest ready time that was available.
//
void DYNAMIC_SCHEDULER_RE_CLASS::TaskSwitch( DYNAMIC_TASK &task )
{
    // start search at next round-robin location
    UINT32 offset = seed++;
    UINT32 N = AllTasks.size();

    // repeat until a suitable task acquired:
    for ( UINT32 retries=10; retries>0; retries-- )
    {
        // first acquire the next ready task,
        // or find the index of the earliest non-ready one:
        INT64 min_time  = task->time();
        INT32 min_index = -1;
        for ( UINT32 i=0; i<N; i++ )
        {
            UINT32 t = ( i + offset ) % N;
            if ( AllTasks[t] != task     &&
                 AllTasks[t]->try_lock()    )
            {
                if ( AllTasks[t]->is_ready() || AllTasks[t]->is_done() )
                {
                    task->unlock();
                    task = AllTasks[t];
                    task->assign_to_this_pthread();
                    return;
                }
                else
                {
                    // task not ready?
                    // Update the earliest-task information,
                    // then release the task and keep looking.
                    if ( AllTasks[t]->time() < min_time )
                    {
                        min_time  = AllTasks[t]->time();
                        min_index = t;
                    }
                    AllTasks[t]->unlock();
                }
            }
        }
        // if you got here, no other available tasks were ready to run.

        // if you found no tasks earlier than the current one,
        // just stick with your current task.
        if ( min_index == -1 ) return;

        // if you did find an earlier task,
        // try acquiring it by restarting the search at its index.
        offset = min_index;
    }
}

//
// ALWAYS-EARLIEST algorithm: find the task with the earliest
// ready time and switch to it.
//
DYNAMIC_SCHEDULER_AE_CLASS::DYNAMIC_SCHEDULER_AE_CLASS()
{
    // the schedule gets instantiated AFTER all tasks have been created and
    // added to AllTasks.  We just need to copy them into the time-ordered list
    vector<DYNAMIC_TASK>::iterator iter;
    for ( iter = AllTasks.begin(); iter != AllTasks.end(); ++iter ) qins( *iter );
}
void DYNAMIC_SCHEDULER_AE_CLASS::qins( DYNAMIC_TASK task )
{
    if ( ! task ) return;
    deque<DYNAMIC_TASK>::iterator iter;
    for ( iter = taskq.begin(); iter != taskq.end(); ++iter )
    {
        if ( (*iter)->time() >= task->time() )
        {
            taskq.insert( iter, task );
            return;
        }
    }
    taskq.push_back( task );
}
DYNAMIC_TASK DYNAMIC_SCHEDULER_AE_CLASS::qrem()
{
    if ( taskq.empty() ) return NULL;
    DYNAMIC_TASK task = taskq.front();
    taskq.pop_front();
    return task;
}
void DYNAMIC_SCHEDULER_AE_CLASS::NotifyExiting( DYNAMIC_TASK task )
{
    lock();
    if ( ! task->ThreadActive() ) exiting = task;
    unlock();
}
void DYNAMIC_SCHEDULER_AE_CLASS::MainThreadTaskRelease( DYNAMIC_TASK &task )
{
    ASIM_SMP_CLASS::SetThreadHandle( ASIM_SMP_CLASS::GetMainThreadHandle() );
    if ( ! task ) return;
    lock();
    qins( task );
    unlock();
    task = NULL;
}
// worker scheduler
void DYNAMIC_SCHEDULER_AE_CLASS::TaskSwitch( DYNAMIC_TASK &task )
{
    if ( nextime > task->time() && ! exiting ) return;
    lock();
    if ( ! task->is_done() )
    {
        task->release_from_pthread();
        qins( task );
        task = exiting ? exiting : qrem();
        exiting = NULL;
        task->assign_to_this_pthread();
        nextime = taskq.front()->time();
    }
    unlock();
}
// master scheduler
void DYNAMIC_SCHEDULER_AE_MASTER_CLASS::TaskSwitch( DYNAMIC_TASK &task )
{
    lock();
    task->release_from_pthread();
    qins( task );
    task =
        ( taskq.front()->is_ready() &&
          taskq.front()->time() == GlobalTimeRing.front()->GetBaseCycle()
        )
        ? qrem()
        : NULL;
    task->assign_to_this_pthread();
    unlock();
}

//
// NULL scheduling algorithm, called by main thread if it is not
// acting as a worker.  Does nothing.
//
void NULL_MASTER_SCHEDULER_CLASS::TaskSwitch( DYNAMIC_TASK &task )
{
}

//
// relinquish any worker task we may have acquired, and
// restore our thread handle, since TaskSwitch() will set it to a worker thread's.
// THIS ROUTINE MUST ONLY BE CALLED BY THE MAIN (clockserver) THREAD!
//
void DYNAMIC_SCHEDULER_CLASS::MainThreadTaskRelease( DYNAMIC_TASK &task )
{
    task->unlock();
    ASIM_SMP_CLASS::SetThreadHandle( ASIM_SMP_CLASS::GetMainThreadHandle() );
    task = NULL;
}


//
// create a software thread to run this task.
// Replaces a like-named function from the base class.
// Note that since this is dynamic scheduling, we don't really
// associate a pthread with a given dynamic task.
// Uses ASIM_SMP_CLASS to actually create the thread.
//
void DYNAMIC_TASK_CLASS::CreatePthread( bool active )
{
    if ( active )
    {
        // increment the number of active threads
        NumPthreads++;
        
        // LOCAL variables to hold return values from ASIM_SMP_CLASS::CreateThread()
        pthread_t               thrd;
        pthread_attr_t          thrd_attr;
        pthread_attr_init    ( &thrd_attr );
        pthread_attr_setscope( &thrd_attr, PTHREAD_SCOPE_SYSTEM );

        ASIM_SMP_CLASS::CreateThread(
            // ignore the thread and attribute values we get back from this call.
            // The DYNAMIC_TASK "thread" field will be set when a pthread
            // acquires this task (not here):
            &thrd, &thrd_attr,
            // the thread will run the ThreadWork() routine,
            // arg is not used and set to NULL:
            ASIM_CLOCKSERVER_THREAD_CLASS::ThreadWork, NULL,
            // pass in the ASIM_SMP_THREAD object that was created:
            GetAsimThreadHandle()
        );

        // don't bother serializing this by waiting for thread active...
    }
    else
    {
        // if not active, create a dormant thread
        ASIM_SMP_CLASS::CreateThread( GetAsimThreadHandle() );
    }
}


//
// destroy the pthread (if any) associated with this dynamic task.
// Called by main thread.
// At the time this routine is called, a pthread may not be actively
// running the task, but as long as there are active pthreads running,
// one will eventually find this task.  This routine therefore lays a trap
// for the next pthread that comes along, and forces it to exit.
//
bool DYNAMIC_TASK_CLASS::DestroyPthread()
{
    if ( --NumPthreads < 0 ) return true;   // if no threads left to destroy, return immediately
    threadForceExit = true;                 // send signal that you want this task to exit
    WorkerScheduler->NotifyExiting( this ); // let the scheduler know we are being killed
    MemBarrier();
    while ( threadForceExit );              // wait for a worker thread to acknowledge the signal
    MemBarrier();
    pthread_join( thread, NULL );           // get "thread" pointer, and wait for thread to exit
    threadActive = false;                   // so parent class destructor does not assert
    return true;
}


//
// End the pthread.  Called from the running pthread.
// Synchronizes with the main thread that just called DestroyPthread().
//
void *DYNAMIC_TASK_CLASS::ExitPthread()
{
    threadForceExit = false;                                // acknowledge getting the signal,
    localDoneTime = GlobalTimeRing.front()->GetBaseCycle(); // notify server we're done,
    pthread_exit(0);                                        // and exit.
    return 0;
}


//
// factory method to create new scheduler of given type.
// Prints an error message and returns NULL if an unknown type name was given.
//
DYNAMIC_SCHEDULER_CLASS *DYNAMIC_SCHEDULER_CLASS::new_scheduler( string type, bool is_master )
{
    if ( type == "Simple"          ) return new DYNAMIC_SCHEDULER_CLASS;
    if ( type == "ReadyToRun"      ) return new DYNAMIC_SCHEDULER_RR_CLASS;
    if ( type == "ReadyOrEarliest" ) return new DYNAMIC_SCHEDULER_RE_CLASS;
    if ( type == "AlwaysEarliest"  ) return is_master
                                         ?  new DYNAMIC_SCHEDULER_AE_MASTER_CLASS
                                         :  new DYNAMIC_SCHEDULER_AE_CLASS;
    VERIFY( 0,
        "Unknown scheduling algorithm: " << type << ", try one of:" << endl <<
            "Simple"          << endl <<
            "ReadyToRun"      << endl <<
            "ReadyOrEarliest" << endl <<
            "AlwaysEarliest " << endl
    );
    return NULL;
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

    // instantiate a scheduler (must be done before we create threads!)
    WorkerScheduler =
              DYNAMIC_SCHEDULER_CLASS::new_scheduler( CLOCKSERVER_SCHEDULING_ALGORITHM, false );
    MasterScheduler =
        ( CLOCKSERVER_THREAD_IS_WORKER )
            ? DYNAMIC_SCHEDULER_CLASS::new_scheduler( CLOCKSERVER_SCHEDULING_ALGORITHM, true  )
            : new NULL_MASTER_SCHEDULER_CLASS;

    // Create the pthreads if requested
    UINT64 nt = 0;
    vector<DYNAMIC_TASK>::iterator
           iter_threads  = DYNAMIC_SCHEDULER_CLASS::AllTasks.begin();
    for( ; iter_threads != DYNAMIC_SCHEDULER_CLASS::AllTasks.end();   ++iter_threads, ++nt )
    {
        (*iter_threads)->CreatePthread( nt < CLOCKSERVER_MAX_WORKER_PTHREADS );
    }
}


//
// return TRUE if and only if there is work to do
// for the given thread at the given event.
//
static bool ThreadActiveOnEvent( ASIM_CLOCKSERVER_THREAD thread, TIME_EVENT_INSTANCE event )
{
    CLOCK_REGISTRY_MODULES_ITERATOR iter;
    for( iter =  event->GetModuleList()->begin();
         iter != event->GetModuleList()->end();
         ++iter )
    {
        if ( thread == (*iter).first->GetClockingThread() ) return true;
    }
    for( iter =  event->GetWriterRMList()->begin();
         iter != event->GetWriterRMList()->end();
         ++iter )
    {
        // !!$#@!!! RATE_MATCHER and ASIM_CLOCKABLE both have different
        // GetClockingThread() routines, that do not inherit from one another.
        // We need to do this cast here to make sure we call the RATE_MATCHER one
        RATE_MATCHER wrm = (RATE_MATCHER)(*iter).first;
        if ( thread == wrm->GetClockingThread() ) return true;
    }
    return false;
}


//
// One iteration of the worker thread loop.
// Assumes that this task is owned by this pthread, and is ready to execute.
//
void DYNAMIC_TASK_CLASS::DoWork()
{
    //
    // find all events at the thread local point in time,
    // and advance my index into the global events list.
    //
    deque<TIME_EVENT_INSTANCE> lClockedEvents;            // list of events at current time point
    INT64 localReadyTime = time();
    while ( is_ready() && time() == localReadyTime )
    {
        lClockedEvents.push_back( get_next_event() );
        advance_event();
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
            if ( this == (*iter).first->GetClockingThread() ) {
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
            if ( this == wrm->GetClockingThread() ) {
                (*iter).second->currentCycle = (*it_event)->GetCycle();
                (*iter).second->Clock();
            }
        }
    }

    //
    // now advance my local "done time".
    // Also advance it up through any subsequent events that this thread doesn't do any work for.
    // This is an optimization to prevent us from re-executing this dynamic task at every
    // time point even if there is no real work to do.  Note that we need to keep "localDoneTime"
    // and the next_event pointer in sync, otherwise deadlock will occur!
    //
    localDoneTime = localReadyTime;
    while ( next_event_valid() && ! ThreadActiveOnEvent( this, get_next_event() ) )
    {
        localDoneTime = time();
        advance_event();
    }
}


//
// fuzzy barrier thread work routine, with lock-free event list,
// and dynamic task switching.
// THE WORKER THREADS EXECUTE THIS.
//
void * ASIM_CLOCKSERVER_THREAD_CLASS::ThreadWork(void* param)
{
    DYNAMIC_TASK task = NULL;
    UINT32 retries = 0;
    while ( ! task->is_done() )
    {
        if ( task->is_ready() )
        {
            WORKER_END_WAIT;
            task->DoWork();
            WORKER_SEND_SIGNAL;
            retries = 0;
        }
        else
        {
            WorkerScheduler->TaskSwitch( task );
            if ( ++retries >= CLOCKSERVER_SPINWAIT_YIELD_INTERVAL )
            {
                sched_yield();
                retries = 0;
            }
            WORKER_BEGIN_WAIT;
        }
    }
    WORKER_CANCEL_WAIT;
    return task->ExitPthread();
}


//
// Fuzzy barrier threaded clock routine, with lock-free event list.
// This basically just advances the ready time, waits for worker threads to catch up,
// updates the event list, and then returns.
// THE MAIN CLOCKSERVER THREAD EXECUTES THIS.
//
UINT64 ASIM_CLOCK_SERVER_CLASS::ThreadedClock() 
{
    //
    // advance simulation time to the timepoint at the head of the events list.
    // This will allow worker threads to proceed forward.
    //
    INT64 currentBaseCycle = GlobalTimeRing.advance_time();
    SERVER_SEND_SIGNAL;

    //
    // spin-wait for all worker threads to finish the current time point,
    // and help out the worker threads while you wait.
    //
    DYNAMIC_TASK task = NULL;
    UINT32 retries = 0;
    while ( GetGlobalDoneTime() < currentBaseCycle )
    {
        if ( task->is_ready() )
        {
            SERVER_CANCEL_WAIT;
            task->DoWork();
            retries = 0;
        }
        else
        {
            MasterScheduler->TaskSwitch( task );
            if ( ++retries >= CLOCKSERVER_SPINWAIT_YIELD_INTERVAL )
            {
                sched_yield();
                retries = 0;
            }
            SERVER_BEGIN_WAIT;
        }
    }
    SERVER_END_WAIT;
    MasterScheduler->MainThreadTaskRelease( task );

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
