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


/*****************************************************************************
*
* @brief Header file for thread safe Cache Manager
*
* @author Carl Beckmann
*
*****************************************************************************/

/*
*   This class is to avoid ending with the same line in 2+ caches 
*   under a multisocket environment.
*   The cache manager should be used for tracking line state in all the caches.
*
*   This version is thread safe, i.e. you can call it from modules running on
*   different threads on the simulator host machine.  This is typically the case
*   if you run a multisocket model, and have different threads assigned to each socket.
*
*   This implementation assumes that although the cache manager might be accessed
*   by different threads, individual lines are always accessed only by a single thread.
*   There is no locking around cache line data structures, only around the cache
*   manager accesses that get you to the line.  (To minimize lock holding time)
*/

#ifndef CACHE_MANAGER_SMP_H
#define CACHE_MANAGER_SMP_H

#include "asim/cache_manager.h"
#include "asim/smp.h"

class CACHE_MANAGER_SMP : public CACHE_MANAGER
{
  protected:
    pthread_mutex_t mutex;       ///< a lock for thread-safe access to cache manager data structures

    CACHE_MANAGER_SMP();         ///< constructor and destructor are private since this is a singleton
    ~CACHE_MANAGER_SMP();
  public:
    // this returns the singleton instance of the thread-safe cache manager
    static CACHE_MANAGER& GetInstance();
    
    // the following functions are reimplemented for thread safety
    virtual void Register(std::string level);
  protected:
    virtual LINE_MANAGER *find_line_manager(std::string level);
};

#endif
