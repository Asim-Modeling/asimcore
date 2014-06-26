/*****************************************************************************
 *
 *
 * @author Oscar Rosell based on Julio Gago's fifo.h
 *
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


/* The differences with Julio's fifo are:
    - Size dynamically determined
    - Closer to stl structures syntax
*/

#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

// ASIM core
#include "asim/syntax.h"
#include "asim/utils.h"

template<class T> 
class circular_queue
{
  public:

    //
    // Constructor.
    //
    circular_queue(UINT32 size);
        
    //
    // Add an object to the tail of the FIFO (there MUST be space available).
    //
    void        push(const T& obj)	{ ASSERTX( count < data.size() ); data[tail] = obj; tail = UP_1_WRAP(tail, data.size()); ++count; }
        
    //
    // Get (but do not remove) an object from the head of the FIFO (there MUST be something in the FIFO).
    //
    T&          front()         { ASSERTX ( count > 0 ); return data[head]; }
    const T&    front() const   { ASSERTX ( count > 0 ); return data[head]; }

    T&          back()          { ASSERTX ( count > 0 ); return data[DOWN_1_WRAP(head + count, data.size())]; }
    const T&    back() const    { ASSERTX ( count > 0 ); return data[DOWN_1_WRAP(head + count, data.size())]; }
        
    //
    // Remove an object from the head of the FIFO (there MUST be something in the FIFO).
    //
    void        pop()           { ASSERTX( count > 0 ); head = UP_1_WRAP(head,data.size()); --count;}

    //
    // Get information about the state of the FIFO: how many objects? is it empty? is it full?
    //
    UINT32      size() const    { return count; }
    bool        empty() const   { return (count == 0);	}
    bool        full() const    { return (count == data.size());	}

  private: 
    std::vector<T> data;

    UINT32 head;
    UINT32 tail;
    UINT32 count;
};

template<class T> 
circular_queue<T>::circular_queue(UINT32 size):
    data(size),
    head(0),
    tail(0),
    count(0)
{
	//
	// empty the queue
	//
}

#endif // CIRCULAR_QUEUE_H
