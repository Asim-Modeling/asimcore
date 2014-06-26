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
 * @file memory.h
 * @author Michael Adler
 * @brief Architecture neutral memory reference descriptors.
 */

#ifndef _MEMORY_REFERENCE_
#define _MEMORY_REFERENCE_

// ASIM core
#include "asim/syntax.h"
#include "asim/mesg.h"


typedef class MEMORY_VIRTUAL_REFERENCE_CLASS *MEMORY_VIRTUAL_REFERENCE;

class MEMORY_VIRTUAL_REFERENCE_CLASS
{
  public:
    MEMORY_VIRTUAL_REFERENCE_CLASS(UINT64 va = 0, UINT64 nBytes = 0) :
        va(va),
        nBytes(nBytes)
    {};

    UINT64 GetVA(void) const { return va; };
    UINT64 GetNBytes(void) const { return nBytes; };

    bool ContainsVA(UINT64 testVA) const;
    
  private:
    UINT64 va;
    UINT64 nBytes;
};


inline bool
MEMORY_VIRTUAL_REFERENCE_CLASS::ContainsVA(UINT64 testVA) const
{
    return (va <= testVA) && (testVA < va + nBytes);
};


#endif // _MEMORY_REFERENCE_
