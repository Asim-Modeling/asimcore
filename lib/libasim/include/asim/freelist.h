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
 * @author Michael Adler
 * @brief Lock-free free list.
 */

#ifndef _FREELIST_
#define _FREELIST_

#include "asim/syntax.h"
#include "asim/atomic.h"


template <typename L_TYPE>
class ASIM_FREE_LIST_ELEMENT_CLASS
{
  private:
    L_TYPE *_freelist_next;

  public:
    ASIM_FREE_LIST_ELEMENT_CLASS() : _freelist_next(NULL) {};

    L_TYPE *GetFreeListNext() const { return _freelist_next; };
    void SetFreeListNext(L_TYPE *next) { _freelist_next = next; };
};
    

template <typename L_TYPE>
class ASIM_FREE_LIST_CLASS
{
  private:
    typedef L_TYPE *L_TYPE_PTR;

    //
    // Head of the free list is two parts.  "list" is the true list head.
    // "txnId" is used to make sure that "list" isn't modified and replaced
    // with the same value by some other thread in the middle of a transaction.
    //
    union HEAD
    {
        struct
        {
	    // this has to be incremented atomically,
	    // and it must be the same size as a pointer,
	    // but it cannot be a class since we're inside an anonymous struct:
            volatile _Atomic_ptr_sized_int txnId;
            L_TYPE_PTR list;
        };

#if __WORDSIZE == 64
        UINT128 full __attribute__ ((aligned(16)));
#else
        UINT64  full __attribute__ ((aligned(8)));
#endif
    };
        
    HEAD head;
    ATOMIC_INT32 size;

  public:
    ASIM_FREE_LIST_CLASS();
    ~ASIM_FREE_LIST_CLASS();

    void Push(L_TYPE *obj);
    L_TYPE *Pop();

    bool Empty(void) const { return head.list == NULL; };
    INT32 Size(void) const { return size; };
};


template <typename L_TYPE>
ASIM_FREE_LIST_CLASS<L_TYPE>::ASIM_FREE_LIST_CLASS()
    : size(0)
{
    // The data structure is expected to be aligned so a 128 bit compare
    // and exchange can work.
    ASSERTX((PTR_SIZED_UINT(&head) & (sizeof(head) - 1)) == 0);
    ASSERTX(sizeof(head.txnId) == sizeof(head.list));

    head.txnId = 0;
    head.list = NULL;
};


template <typename L_TYPE>
ASIM_FREE_LIST_CLASS<L_TYPE>::~ASIM_FREE_LIST_CLASS()
{
    ASSERTX(head.list == NULL);
};


template <typename L_TYPE>
void ASIM_FREE_LIST_CLASS<L_TYPE>::Push(
    L_TYPE *obj)
{
    if (obj == NULL) return;
    ASSERTX(obj->GetFreeListNext() == NULL);

    L_TYPE *oldList;
    do
    {
        oldList = head.list;
        obj->SetFreeListNext(oldList);
    }
    while (! CompareAndExchange((PTR_SIZED_UINT*)&head.list,
                                (PTR_SIZED_UINT)oldList,
                                (PTR_SIZED_UINT)obj));

    size++;
};


template <typename L_TYPE>
L_TYPE *ASIM_FREE_LIST_CLASS<L_TYPE>::Pop()
{
    HEAD oldHead, newHead;
    do
    {
        oldHead.txnId = __exchange_and_add((volatile _Atomic_ptr_sized_int *)(&head.txnId),1) + 1;
        oldHead.list = head.list;

        if (oldHead.list == NULL)
        {
            return NULL;
        }

        newHead.txnId = oldHead.txnId;
        newHead.list = oldHead.list->GetFreeListNext();
    }
    while (! CompareAndExchange(&head.full, oldHead.full, newHead.full));

    size--;

    oldHead.list->SetFreeListNext(NULL);
    return oldHead.list;
};


#endif // _FREELIST_
