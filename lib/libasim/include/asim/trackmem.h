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
 * @author Shubu Mukherjee
 * @brief Debugging support: track memory allocated by different routines.
 */

#ifndef _trackmem_h
#define _trackmem_h

// ASIM core
#include "asim/syntax.h"
#include "asim/stateout.h"

#if (defined(HOST_LINUX) && defined(__GNUC__))
// memory tracking works only with Tru64 because the strack tracing is OS-specific
// for some reason, this seg faults with the gem compiler in strlen (problem with 
// parsing mangled c++ names?), but works fine with gnuc (?) -Shubu
#else
// #define TRACK_MEMORY_ALLOCATION 	1
#endif

#ifdef TRACK_MEMORY_ALLOCATION

void StackTraceInit(char * filename); 
void DumpMemAllocInfo(STATE_OUT stateOut); 
char *StackTraceParentProc(); 

#else

#define StackTraceInit(filename)
#define DumpMemAllocInfo(stateOut)	

char *StackTraceParentProc(); 

#endif

#endif 
