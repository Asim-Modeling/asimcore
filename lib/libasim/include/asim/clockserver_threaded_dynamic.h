*
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

#ifndef __CLOCKSERVER_THREADED_DYNAMIC_H__
#define __CLOCKSERVER_THREADED_DYNAMIC_H__

// include the clock server base code
#include "asim/restricted/clockserver.h"

// include the lockfree event list classes
#include "time_events_ring.h"


//
// Dynamic scheduling task object class.
//
// A task is all the work that must be performed for a given module on a given event.
// This might consist of simply the Clock() callback, but it may also include any
// write rate matcher clocks, which must happen after the Clock() callback, but at
// the reader frequency.
//
typedef
class CLOCKSERVER_DYNAMIC_TASK_CLASS *CLOCKSERVER_DYNAMIC_TASK;
class CLOCKSERVER_DYNAMIC_TASK_CLASS
{
  private:
    TIME_EVENTS_RING_CLASS::ITERATOR next_event;    // index into next event in global time event list
    CLOCK_REGISTRY                   clock_event;   // the main clock event for the module
    ASIM_CLOCKABLE                   module;        // the clockable module this task owns
    CLOCK_CALLBACK_INTERFACE         callback;      // the clock callback for this module
    INT64                            next_time;     // predicted next active time for this task

    static deque<CLOCKSERVER_DYNAMIC_TASK> queue;   // the global task queue
    static pthread_mutex_t           mutex;         // a lock to guarantee exlusive access
    static vector<INT64>             busy_times;    // array of busy times, one per active thread
    static volatile INT64            busy_time;     // the global busy time, min of all of the above & queue
  public:
    CLOCKSERVER_DYNAMIC_TASK_CLASS(                 // constructor
                    ASIM_CLOCKABLE           mod,   //     the module owned by this task
                    CLOCK_CALLBACK_INTERFACE cb,    //     the clock callback for this module
                    CLOCK_REGISTRY           evt    //     the event that clocks this module
                )
    :   next_event( &GlobalTimeRing ), clock_event(evt), module(mod), callback(cb), next_time(0)
    {
        queue.push_back( this );
    };
    bool is_ready()                                 // is this task currently ready to execute?
    {
        return next_event.is_ready();
    };
    static void acquire()                           // lock or unlock the global task queue
    {
        pthread_mutex_lock( &mutex );
    };
    static void release()
    {
        pthread_mutex_unlock( &mutex );
    };
    static INT64 get_busy_time()                    // get the earliest time workers have not finished simulating
    {
        return busy_time;
    };
    INT64 get_next_time()                           // get the next time at which this task will become active
    {                                               //    (it's ok to err on the side of being too early here,
        return                                      //     in the worst case we'll get woken up too soon.)
            ( next_event.is_valid() )
            ?   next_event.time()
            :   next_time;
    };
    static CLOCKSERVER_DYNAMIC_TASK
         fetch_next( INT32 thd_idx );               // get a task from the ready queue
    static CLOCKSERVER_DYNAMIC_TASK
         fetch_next_if_ready( INT32 thd_idx );      // get a task only if it is ready to run
    void do_work();                                 // clock the module, rate matchers, advance local clock
    void put_back( INT32 thd_idx );                 // finish a task and re-queue it
    bool has_work();                                // does the current event have work for this task?
    static void init_queue();                       // initialize the global task queue
};


#endif
