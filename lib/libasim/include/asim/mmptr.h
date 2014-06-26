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
 * @author Nathan Binkert
 * @brief Automatic reference counting on pointers to MM objects.
 */

// generic
#include <iostream>

// ASIM core
#include "asim/syntax.h"
#include "asim/ioformat.h"

namespace iof = IoFormat;
using namespace iof;
using namespace std;

template <class Type> class ASIM_MM_CLASS;

#ifndef _MMPTR_
#define _MMPTR_

template <class Type>
class mmptr
{
  private:
    Type *ptr;
    void copy(Type *p);
    bool del();

  public:
    mmptr() { ptr = NULL; }
    mmptr(Type *p) { copy(p); }
    mmptr(const mmptr &mmp) { copy(mmp.ptr); }
    ~mmptr()
    {
        bool killObj = del();
        Type *oldPtr = ptr;

        ptr = NULL;

        if (killObj)
        {
            ((ASIM_MM_CLASS<Type>*)oldPtr)->LastRefDropped();
        }
    }

    Type * ptr_value();

    Type *operator->() const { return ptr; }
    Type &operator*() const { return *ptr; }
    operator Type *() const { return ptr; }
     
    Type *operator=(const mmptr &mmp) const { return ptr; }

    mmptr &operator=(Type *p) {
        if (p != ptr) {
            bool killObj = del();
            Type *oldPtr = ptr;

            copy(p);

            if (killObj)
            {
                ((ASIM_MM_CLASS<Type>*)oldPtr)->LastRefDropped();
            }
        }

        return *this;
    }

    mmptr &operator=(const mmptr &mmp) {
        return operator=(mmp.ptr);
    }
};  

template <class Type>
Type * 
mmptr<Type>::ptr_value() 
{ 
    return ptr; 
}

template <class Type>
inline void 
mmptr<Type>::copy(Type *p) 
{

    ptr = p;
    if(ptr) 
    {
        ((ASIM_MM_CLASS<Type>*)ptr)->IncrRef();
    }
}
 
template <class Type>
inline bool
mmptr<Type>::del() 
{
    bool killObj = false;

    if(ptr) 
    {
        //
        // DecrRef returns the reference count following the decrement.  If
        // it reaches 0 return true so the caller knows to kill the object
        // after clearing any pointers to it.
        //
        if (((ASIM_MM_CLASS<Type>*)ptr)->DecrRef() <= 0)
        {
            killObj = true;
        }
    }

    return killObj;
}

#endif //_MMPTR_
