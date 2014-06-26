/****************************************************************************
 *
 * @brief  Support for thread-safe execution.
 * 			This provides basic support for mutexes (critical sections),
 * 			atomic data types, and thread-local variables.
 *
 * @author carl.j.beckmann@intel.com
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
#ifndef __ASIM_THREADSAFE_H__
#define __ASIM_THREADSAFE_H__

#if MAX_PTHREADS > 1

//////////////////////////////////////////
//
// Threading-enabled version of the macros,
// based on pthread mutexes.
//
//////////////////////////////////////////

#include "asim/atomic.h"

#define CM_ATOMIC_REGION \
    static pthread_mutex_t _mutex_ = PTHREAD_MUTEX_INITIALIZER; \
    SEQUENTIAL _scoped_lock_( _mutex_ );

#define CM_GLOBAL_ATOMIC_RESOURCE( _lockname_ ) \
    static pthread_mutex_t _lockname_ ## _storage = PTHREAD_MUTEX_INITIALIZER; \
    static pthread_mutex_t _lockname_ = & _lockname_ ## _storage;

#define CM_MEMBER_ATOMIC_RESOURCE( _lockname_ ) \
    pthread_mutex_t * _lockname_;

#define CM_MEMBER_ATOMIC_RESOURCE_INIT( _lockname_ ) \
    _lockname_ = new pthread_mutex_t; \
    pthread_mutex_init( _lockname_, NULL );

#define CM_GLOBAL_ATOMIC_RESOURCE_RECURSIVE( _lockname_ ) \
    static pthread_mutex_t _lockname_ ## _storage = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP; \
    static pthread_mutex_t _lockname_ = & _lockname_ ## _storage;

#define CM_ATOMIC_RESOURCE( _lockname_ ) \
    SEQUENTIAL _scoped_lock_( * _lockname_ );

#define CM_ATOMIC_SCALAR( _type_, _name_ ) _type_ _name_ \
    ERROR! ATOMIC_SCALAR MACRO NOT IMPLEMENTED YET IN asim/threadsafe.h !!!

#define CM_THREAD_LOCAL \
    __thread

#else

//////////////////////////////////////////
//
// not compiling for parallel execution?
// Just optimize away these macros.
//
//////////////////////////////////////////

#define CM_ATOMIC_REGION
#define CM_GLOBAL_ATOMIC_RESOURCE( _lockname_ )
#define CM_MEMBER_ATOMIC_RESOURCE( _lockname_ )
#define CM_MEMBER_ATOMIC_RESOURCE_INIT( _lockname_ )
#define CM_GLOBAL_ATOMIC_RESOURCE_RECURSIVE( _lockname_ )
#define CM_ATOMIC_RESOURCE( _lockname_ ) 
#define CM_ATOMIC_SCALAR( _type_, _name_ ) _type_ _name_
#define CM_THREAD_LOCAL

#endif

#endif // __ASIM_THREADSAFE_H__
