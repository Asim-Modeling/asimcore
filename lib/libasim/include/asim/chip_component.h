/*****************************************************************************
 *
 * @brief Header file for Chip Component
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

#ifndef _CHIP_COMPONENT_
#define _CHIP_COMPONENT_

// ASIM core
#include "asim/syntax.h"
#include "asim/module.h"

// ASIM public modules


typedef class ASIM_CHIP_COMPONENT_CLASS *ASIM_CHIP_COMPONENT;

class ASIM_CHIP_COMPONENT_CLASS : public ASIM_MODULE_CLASS
{
  public:

    ASIM_CHIP_COMPONENT_CLASS(ASIM_MODULE parent, const char* const name);

    ~ASIM_CHIP_COMPONENT_CLASS();

    // This is an abstract class
    virtual void Clock(const UINT64 cycle) =0;
};

inline 
ASIM_CHIP_COMPONENT_CLASS::ASIM_CHIP_COMPONENT_CLASS(
    ASIM_MODULE parent, 
    const char* const name)
    : ASIM_MODULE_CLASS(parent, name)
{}

inline 
ASIM_CHIP_COMPONENT_CLASS::~ASIM_CHIP_COMPONENT_CLASS()
{}

#endif /* _CHIP_COMPONENT_ */
