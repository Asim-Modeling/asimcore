/****************************************************************************
 *
 * @brief  Support for thread-safe execution.
 * 			This provides basic support for mutexes (critical sections),
 * 			atomic data types, and thread-local variables.
 *
 * @author carl.j.beckmann@intel.com
 *
 *Copyright (C) 2003-2006 Intel Corporation
 *
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
