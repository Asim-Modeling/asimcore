/*
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


#include <deque>

template <class T, unsigned N>
class CIRCULAR_QUEUE_CLASS {
    deque<T> queue;

    UINT32 max_size;

  public:
    CIRCULAR_QUEUE_CLASS();
    virtual ~CIRCULAR_QUEUE_CLASS() {}
    
    void Reset() { queue.clear(); }

    bool Add(T &d);
    bool Enqueue(T &d) { return Add(d); }

    bool Remove(T &d);
    bool Dequeue(T &d) { return Remove(d); }

    bool Peek(T &d);


    UINT32 Size()     { return queue.size(); }
    UINT32 Capacity() { return max_size; }

    bool IsFull()  { return (queue.size() == max_size); }
    bool IsEmpty() { return (queue.size() == 0); }

};

//
//  Constructor
//
template <class T, unsigned N>
CIRCULAR_QUEUE_CLASS<T,N>::
CIRCULAR_QUEUE_CLASS() 
{
    max_size = N;
    queue.resize(max_size);
}


template <class T, unsigned N>
bool
CIRCULAR_QUEUE_CLASS<T,N>::
Add(T &d)
{
    if (queue.size() < max_size) {
	queue.push_back(d);
    }
    else {
	return false;
    }

    return true;
}


template <class T, unsigned N>
bool
CIRCULAR_QUEUE_CLASS<T,N>::
Remove(T &d)
{
    if (!queue.empty()) {
	d = *(queue.begin());
	queue.pop_front();
    }
    else {
	return false;
    }

    return true;
}


template <class T, unsigned N>
bool
CIRCULAR_QUEUE_CLASS<T,N>::
Peek(T &d)
{
    if (!queue.empty()) {
	d = *(queue.begin());
    }
    else {
	return false;
    }

    return true;
}

