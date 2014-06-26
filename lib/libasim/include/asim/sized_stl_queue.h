/*****************************************************************************
 *
 * @brief Header file for SIZED_STL_QUEUE
 *
 * @author Pritpal Ahuja
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

#ifndef _SIZED_STL_QUEUE_
#define _SIZED_STL_QUEUE_

#include <queue>

template<class T>
class SIZED_STL_QUEUE_CLASS : public queue<T>
{
    const UINT32 myMaxSize;

  public:
    SIZED_STL_QUEUE_CLASS(const UINT32 max_size);

    bool push(T t);
    bool full();
};


template<class T>
SIZED_STL_QUEUE_CLASS<T>::SIZED_STL_QUEUE_CLASS(const UINT32 max_size)
    : queue<T>(), myMaxSize(max_size)
{
}


template<class T> 
inline bool
SIZED_STL_QUEUE_CLASS<T>::push(T t)
{
    if (this->size() < myMaxSize)
    {
        queue<T>::push(t);
        return true;
    }

    return false;
}


template<class T>
inline bool
SIZED_STL_QUEUE_CLASS<T>::full()
{
    return (this->size() == myMaxSize);
}


#endif // _SIZED_STL_QUEUE_
