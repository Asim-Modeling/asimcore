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

/**
 * @file
 * @author Julio Gago 
 * @brief
 */

#ifndef _FIFO_H
#define _FIFO_H

// generic
#include <stdio.h>

// ASIM core
#include "asim/syntax.h"
#include "asim/utils.h"

/**
 * This class provides elemental and fast FIFO support for fixed-size queues
 * by using a static array and a few indexes. Although it may seem too much
 * simple to deserve a base class, it is used very often (the CBOX has tons
 * of simple FIFOs) both for object fifo's and free lists. Furthermore, the
 * universally known GET/PUT FIFO mechanisms are a potential source of bugs,
 * so it's always safer to encapsulate them.
 */
template<class T, UINT32 N> class fifo
{
  private: 

	UINT32 head;
	UINT32 tail;
	UINT32 count;
	  
	T data[N];

  public:

	//
	// Constructor.
	//
	fifo();
	
	//
	// Add an object to the tail of the FIFO (there MUST be space available).
	//
	void			Put(T obj)		{ ASSERTX( count < N ); data[tail] = obj;	tail = (tail + 1) % N; count++; }
	
	//
	// Get (and remove) an object from the head of the FIFO (there MUST be something in the FIFO).
	//
	T			Get()			{ ASSERTX( count > 0 ); T ret = data[head];	head = (head + 1) % N; count--; return ret; }
	
	//
	// Get (but do not remove) an object from the head of the FIFO (there MUST be something in the FIFO).
	//
	T			Peek()			{ ASSERTX ( count > 0 ); return data[head]; }
	
	//
	// Remove an object from the head of the FIFO (there MUST be something in the FIFO).
	//
	void			Drop()			{ ASSERTX( count > 0 ); 			head = (head + 1) % N; count--; return; }

	//
	// Get information about the state of the FIFO: how many objects? is it empty? is it full?
	//
	UINT32			Count()			{ return count; 	}
	bool			Empty()			{ return (count == 0);	}
	bool			Full()			{ return (count == N);	}
};

template<class T, unsigned N> 
fifo<T,N>::fifo()
{
	//
	// empty the queue
	//
	count = 0;
	head = 0;
	tail = 0;
}

#endif // _FIFO_H
