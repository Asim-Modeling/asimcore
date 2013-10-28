/****************************************************************************
 *
 *
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
#ifndef __TIME_EVENTS_RING_H__
#define __TIME_EVENTS_RING_H__


/**
 * @author Carl Beckmann
 **/


//
// This class represents an instance of a time event (a clock edge).
// It plays the role of the CLOCK_REGISTRY class in the old Asim clock server,
// except that you can have multiple instances of a given clock in the event
// list at once.  This is necessary to allow the lookahead window in the fuzzy
// barrier clockserver to be greater than a single clock cycle.
//
// This class proxies the CLOCK_REGISTRY class, but provides a different
// nBaseCycle field for every instance.
//
typedef
class TIME_EVENT_INSTANCE_CLASS *TIME_EVENT_INSTANCE;
class TIME_EVENT_INSTANCE_CLASS
{
  private:
    const CLOCK_REGISTRY parent;     // the recurring event we are an instance of
    UINT64 nBaseCycle;               // the next event time, in base frequency cycles
    UINT64 nCycle;                   // Local cycle counter to this frequency

  public:
    // construct an event instance from a recurring event
    TIME_EVENT_INSTANCE_CLASS( CLOCK_REGISTRY p )
      : parent(p),
        // initialize the cycle counts from the parent, and the number
        // of event instances previously instantiated for this parent:
        nBaseCycle( p->nBaseCycle + p->nEventInstances * p->nStep ),
        nCycle    ( p->nCycle     + p->nEventInstances            )
    {
        p->nEventInstances++;        // increment the number of instances belonging to the parent
    };
    //
    // return pointers to parent's module or write rate matcher callback lists
    //
    vector< pair<ASIM_CLOCKABLE, CLOCK_CALLBACK_INTERFACE> >
      *GetModuleList()
    {
        return & parent->lModules;
    };
    vector< pair<ASIM_CLOCKABLE, CLOCK_CALLBACK_INTERFACE> >
      *GetWriterRMList()
    {
        return & parent->lWriterRM;
    };
    //
    // get local frequency and base frequency cycle counts
    //
    UINT64 GetCycle()
    {
        return nCycle;
    };
    UINT64 GetBaseCycle()
    {
        return nBaseCycle;
    };
    UINT64 GetStep()
    {
        return parent->nStep;
    };
    //
    // access the parent recurring event
    //
    CLOCK_REGISTRY GetParent()
    {
        return parent;
    };
    //
    // Step this event instance.
    // This routine increments the cycle counts for this event instance,
    // skipping over all the other event instances for this recurring event.
    // It also increments the cycle counts for the parent recurring event.
    // This routine must only be called for the oldest instance of the parent event.
    //
    void AdvanceCycle()
    {
        // make sure this is the oldest instance
        VERIFYX( parent->nBaseCycle == nBaseCycle );
        VERIFYX( parent->nCycle     == nCycle     );
        // step the cycle counters by the number of instances
        // for this recurring event:
        nCycle     += parent->nEventInstances;
        nBaseCycle += parent->nEventInstances * parent->nStep;
        // increment the parent recurring event's cycle counters:
        parent->nCycle++;
        parent->nBaseCycle += parent->nStep;
    };
};

// for iterating over lists (deque's) of event instances
typedef deque<TIME_EVENT_INSTANCE>::iterator TIME_EVENT_INSTANCE_ITERATOR;


//
// this class is a lock-free time event list.
// It is a ring buffer of CLOCK_REGISTRY object pointers that worker threads
// access using persistent iterators.  The clock server main thread updates
// the events in the list, and updates may happen concurrently with reads
// as long as disjoint elements are accessed.
//
typedef
class TIME_EVENTS_RING_CLASS *TIME_EVENTS_RING;
class TIME_EVENTS_RING_CLASS
{
  private:
    //
    // ring object data
    //
    static const size_t         MAX_TIME_EVENTS_RING_ELEMENTS = 256;
    TIME_EVENT_INSTANCE element[MAX_TIME_EVENTS_RING_ELEMENTS];
    // these are volatile because they are used as synchronization variables,
    // written by the clockserver thread and read by the worker threads:
    volatile size_t head;        // points to first valid event in the list
    volatile size_t limit;       // points just beyond last event in lookahead window
    volatile size_t tail;        // points just beyond last valid event in the list
    UINT64 lookahead;            // the amount of lookahead in base frequency cycles:

  public:
    // a ring object iterator is an index into the time events ring
    // and is constructed by passing a pointer to the ring object.
    // It can be dereferenced, or incremented, or compared.
    class ITERATOR
    {
      private:
        TIME_EVENTS_RING ring;   // the ring object we are indexing into
        size_t           index;  // index into the ring's array
      public:
        ITERATOR(TIME_EVENTS_RING r) : ring( r ) { index = ring->head;                   };
        TIME_EVENT_INSTANCE& operator* ()        { return ring->element[index];          };
        operator size_t()                        { return index;                         };
        INT64 time()                             { return ( * (*this) )->GetBaseCycle(); };
        // prefix ++ increments the iterator and returns its new value
        size_t operator++ ()
        {
            index++;
            if ( index == MAX_TIME_EVENTS_RING_ELEMENTS )
            {
                index = 0;
            }
            return index;
        };
        // does this iterator point to a valid element,
        // i.e. something between the head and the tail pointer?
        bool is_valid()
        {
            // Read these exactly once, and then compare using the snapshot copies.
            // The snapshot is necessary to guarantee an atomic comparison since the
            // head and tail pointers are updated by the clockserver thread, but
            // this routine may be called by the worker threads.
            size_t snapshot_head = ring->head;
            size_t snapshot_tail = ring->tail;
            return
                ( snapshot_tail >= snapshot_head )
                ?   ( ( index >= snapshot_head ) && ( index < snapshot_tail ) )
                :   ( ( index >= snapshot_head ) || ( index < snapshot_tail ) )
            ;
        };
        // does this iterator point to an element that is ready to execute,
        // i.e. one between the head and the lookahead limit pointer?
        bool is_ready()
        {
            size_t snapshot_head  = ring->head;
            size_t snapshot_limit = ring->limit;
            return
                ( snapshot_limit >= snapshot_head )
                ?   ( ( index >= snapshot_head ) && ( index < snapshot_limit ) )
                :   ( ( index >= snapshot_head ) || ( index < snapshot_limit ) )
            ;
        };
    };

    //
    // ring object methods
    //
    TIME_EVENTS_RING_CLASS() : head(0), limit(0), tail(0), lookahead(0) {        };
    void set_lookahead( UINT64 la )                      { lookahead = la;       }
    TIME_EVENT_INSTANCE front()                          { return element[head]; };
    // return a pointer to the event on the head of the list,
    // and remove the event from the list
    TIME_EVENT_INSTANCE pop_front()
    {
        if (head == tail)
        {
            return NULL;
        }
        TIME_EVENT_INSTANCE ret = front();
        // we need to write the new head value atomically
        size_t new_head = head;
        new_head++;
        if ( new_head == MAX_TIME_EVENTS_RING_ELEMENTS )
        {
            new_head = 0;
        }
        head = new_head;
        return ret;
    };
    // insert a recurring event into the ring at the correct time point.
    // This is called by the clockserver initialization routine,
    // with a CLOCK_REGISTRY recurring event object as an argument.
    // This creates enough event instances to span the fuzzy barrier
    // lookahead window and inserts them into the ring.
    void insert(CLOCK_REGISTRY event)
    {
        int i;
        UINT64 span=0;
        for ( i = 0;
              // the number of instances has to be 1 more than the minimum to span
              // the lookahead window, so that every item in the ring always points
              // at a valid event instance even if it is not ready to execute:
              span <= lookahead + event->nStep;
              i++, span += event->nStep       )
        {
            insert( new TIME_EVENT_INSTANCE_CLASS( event ) );
        };
        //DEBUG: cout << i << " EVENT INSTANCES CREATED FOR CLOCK DOMAIN " << event->clockDomain->name << endl;
    };
    // reinsert an event instance into the ring at the correct time point.
    // there are no locks here because we assume that the insertion
    // point is past the lookahead limit, so it will only affect the
    // portion of the ring that worker threads do not access.
    void insert(TIME_EVENT_INSTANCE event)
    {
        // find the insertion point
        INT64 etime = event->GetBaseCycle();
        ITERATOR i( this );
        for ( ; i.is_valid() && i.time() <= etime; ++i ) ;
        ASSERT( !i.is_ready(), "Event insertion attempted into lookahead window!\n" );
        // insert the event
        TIME_EVENT_INSTANCE prev = *i;
        *i = event;
        // move everything after the insertion point up one
        while ( i.is_valid() )
        {
            ++i;
            TIME_EVENT_INSTANCE next = *i;
            *i = prev;
            prev = next;
        }
        // update the tail pointer.
        // The memory barrier is needed here because we have to make sure that all the events
        // have really been committed to ring memory before updating the tail pointer,
        // which is a volatile synchronization variable read by other threads.
        MemBarrier();
        tail = ++i;
        ASSERT( tail != head, "Event list overflow!\n" );
    };
    // advance simulation time to the timepoint at the head of the events list.
    // This will allow worker threads to proceed forward.
    // We do this by simply advancing the lookahead limit pointer.
    // Return the time that we advanced to.
    INT64 advance_time()
    {
        ITERATOR i( this );
        ASSERT( i.is_valid(), "advance_time() called on empty time events ring!\n" );
        // get the time at the head of the list
        INT64 now = i.time();
        // find the time point that is just beyond the current time plus lookahead
        INT64 next = now + lookahead;
        while ( i.is_valid() &&
                i.time() <= next )
        {
            ++i;
        }
        // set the lookahead limit index to this.
        // This is the actual synchronization assignment!
        // No memory barrier is needed here, because the one in insert() already
        // guarantees that ring memory has valid event data in it.
        limit = i;
        // return the current time to the caller
        return now;
    };
};

// there is one global time-events list
extern TIME_EVENTS_RING_CLASS GlobalTimeRing;

#endif
