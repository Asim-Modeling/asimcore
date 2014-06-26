/*
 * **********************************************************************
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

/*****************************************************************************
*
* @brief Source file for thread safe Cache Manager
*
* @author Carl Beckmann
*
*****************************************************************************/

#include "asim/cache_manager_smp.h"


//
// the constructor and destructor need to initialize, or destroy, the threading lock
//
CACHE_MANAGER_SMP::CACHE_MANAGER_SMP()
{
    pthread_mutex_init( &mutex, NULL );
}

CACHE_MANAGER_SMP::~CACHE_MANAGER_SMP()
{
    pthread_mutex_destroy( &mutex );
}

//
// macros for entering/exiting the critical section
//
#define ENTER_CACHE_MANAGER pthread_mutex_lock  ( &mutex )
#define LEAVE_CACHE_MANAGER pthread_mutex_unlock( &mutex )

//
// return a ref to the singleton instance of this class
//
CACHE_MANAGER&
CACHE_MANAGER_SMP::GetInstance()
{
    static CACHE_MANAGER_SMP the_manager;
    return the_manager;
}

//
// these methods simply call the inherited methods inside a critical section
//
void
CACHE_MANAGER_SMP::Register(std::string level)
{
    ENTER_CACHE_MANAGER;
    CACHE_MANAGER::Register( level );
    LEAVE_CACHE_MANAGER;
}

CACHE_MANAGER::LINE_MANAGER *
CACHE_MANAGER_SMP::find_line_manager(std::string level)
{
    LINE_MANAGER *line_manager;
    ENTER_CACHE_MANAGER;
    line_manager = CACHE_MANAGER::find_line_manager( level );
    LEAVE_CACHE_MANAGER;
    return line_manager;
}
