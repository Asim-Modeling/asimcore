/**************************************************************************
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
 * @file dynamic_array.h
 * @author Michael Adler
 * @brief Simple class for managing an array of unknown initial size.
 */

#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_


// ASIM core
#include "asim/syntax.h"

#ifndef SOFTSDV_STUB
#include "asim/mesg.h"
#endif

//
// This class is intended to be an easy replacement for fixed size arrays.
// It can be accessed like the old array.  Only the declaration of the
// array needs to be changed an a call to the Init() method added when
// the number of elements is known.
//

template <class DATA>
class DYNAMIC_ARRAY_CLASS
{
  public:
    DYNAMIC_ARRAY_CLASS(UINT32 elem = 0);
    ~DYNAMIC_ARRAY_CLASS();

    void Init(UINT32 elem);
    bool IsInitialized(void) const;

    DATA& operator[](UINT32 idx) const;

  private:
    UINT32 nElements;
    DATA *data;
};


template <class DATA>
inline
DYNAMIC_ARRAY_CLASS<DATA>::DYNAMIC_ARRAY_CLASS(UINT32 elem) : nElements(0), data(NULL)
{
    if (elem)
    {
        Init(elem);
    }
};


template <class DATA>
inline
DYNAMIC_ARRAY_CLASS<DATA>::~DYNAMIC_ARRAY_CLASS()
{
    if (nElements)
    {
        delete[] data;
        nElements = 0;
    }
};


template <class DATA>
inline void
DYNAMIC_ARRAY_CLASS<DATA>::Init(UINT32 elem)
{
    ASSERTX(nElements == 0);
    ASSERTX(elem > 0);

    nElements = elem;
    data = new DATA[elem];

    VERIFY(data != NULL, "Out of memory");
};


template <class DATA>
inline bool
DYNAMIC_ARRAY_CLASS<DATA>::IsInitialized(void) const
{
    return (nElements != 0);
};


template <class DATA>
inline DATA&
DYNAMIC_ARRAY_CLASS<DATA>::operator[](UINT32 idx) const
{
    ASSERTX(idx < nElements);
    return data[idx];
};


#endif // _DYNAMIC_ARRAY_H_
