/*****************************************************************************
 *
 * @brief Definitions to support pthreads implemenation.
 *
 * @author Kenneth Barr, Ramon Matas Navarro
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


#include "asim/smp.h"
#include "asim/atomic.h"
#include <iostream>
#include "asim/mesg.h"


ATOMIC32_CLASS ASIM_SMP_THREAD_HANDLE_CLASS::threadUidGen = 0;

ASIM_SMP_THREAD_HANDLE ASIM_SMP_CLASS::mainThread;
pthread_key_t ASIM_SMP_CLASS::threadLocalKey;
UINT32 ASIM_SMP_CLASS::maxThreads = 0;
ATOMIC32_CLASS ASIM_SMP_CLASS::activeThreads = 0;

#ifdef TLS_AVAILABLE
__thread INT32 ASIM_SMP_RunningThreadNumber = 0;
#endif


ASIM_SMP_THREAD_HANDLE_CLASS::ASIM_SMP_THREAD_HANDLE_CLASS()
    : threadCreateArg(NULL),
      threadNumber(-1)
{
    threadId = threadUidGen++;
};


void
ASIM_SMP_THREAD_HANDLE_CLASS::NoteThreadActive(INT32 tNum)
{
    VERIFYX(threadNumber == -1);
    threadNumber = tNum;
};



void
ASIM_SMP_CLASS::Init(
    UINT32 staticMaxThreads,
    UINT32 dynamicMaxThreads)
{
    //
    // Dynamic maximum number of threads configured in the model.  If
    // dynamicMaxThreads is 0 then use staticMaxThreads.
    //
    if (dynamicMaxThreads > 0)
    {
        VERIFYX(dynamicMaxThreads <= staticMaxThreads);
        maxThreads = dynamicMaxThreads;
    }
    else
    {
        maxThreads = staticMaxThreads;
    }

    mainThread = new ASIM_SMP_THREAD_HANDLE_CLASS();
    mainThread->NoteThreadActive(activeThreads++);
    VERIFY(mainThread->GetThreadId() == 0, "ASIM_SMP_CLASS::Init() called more than once");
    VERIFYX(0 == pthread_key_create(&threadLocalKey, NULL));
    SetThreadHandle(mainThread);
    VERIFYX(GetRunningThreadNumber() == 0);
}


void
ASIM_SMP_CLASS::CreateThread(
    pthread_t *thread,
    pthread_attr_t *attr,
    void *(*start_routine)(void *),
    void *arg,
    ASIM_SMP_THREAD_HANDLE threadHandle)
{
    threadHandle->start_routine = start_routine;
    threadHandle->threadCreateArg = arg;
    threadHandle->threadNumber = activeThreads++;

    VERIFY(UINT32(threadHandle->threadNumber) < ASIM_SMP_CLASS::GetMaxThreads(),
           "Thread limit exceeded (" << ASIM_SMP_CLASS::GetMaxThreads() << ")");

    VERIFYX(0 == pthread_create(thread, attr, &ThreadEntry, threadHandle));
}


//
// create a "dormant" thread.
// Do not allocate a pthread here, but still give it an active thread ID.
//
void
ASIM_SMP_CLASS::CreateThread(
    ASIM_SMP_THREAD_HANDLE threadHandle)
{
    threadHandle->start_routine   = NULL;
    threadHandle->threadCreateArg = NULL;
    threadHandle->threadNumber    = activeThreads++;

    VERIFY(UINT32(threadHandle->threadNumber) < ASIM_SMP_CLASS::GetMaxThreads(),
           "Thread limit exceeded (" << ASIM_SMP_CLASS::GetMaxThreads() << ")");
}


void *
ASIM_SMP_CLASS::ThreadEntry(void *arg)
{
    //
    // All threads enter through this function...
    //

    ASIM_SMP_THREAD_HANDLE threadHandle = ASIM_SMP_THREAD_HANDLE(arg);

    SetThreadHandle(threadHandle);

    cout << "Thread " << GetRunningThreadNumber() << " created." << endl;
    
    // Call the real entry function
    return (*(threadHandle->start_routine))(threadHandle->threadCreateArg);
}


ASIM_SMP_THREAD_HANDLE
ASIM_SMP_CLASS::GetThreadHandle(void)
{
    ASIM_SMP_THREAD_HANDLE threadHandle =
        (ASIM_SMP_THREAD_HANDLE) pthread_getspecific(threadLocalKey);

    VERIFYX(threadHandle != NULL);
    return threadHandle;
}


//
// in dynamic scheduling schemes, we may have pthreads switching between
// different ASIM_SMP_THREAD tasks.  Thus you might allocate more ASIM_SMP_THREAD's
// than there are pthreads running.  When a pthread starts running an ASIM_SMP_THREAD,
// it should call this routine to swithc the thread-local variable to point to
// the currently running ASIM_SMP_THREAD.
//
void
ASIM_SMP_CLASS::SetThreadHandle(ASIM_SMP_THREAD_HANDLE threadHandle)
{
#ifdef TLS_AVAILABLE
    ASIM_SMP_RunningThreadNumber = threadHandle->threadNumber;
#endif

    VERIFYX(0 == pthread_setspecific(threadLocalKey, threadHandle));
}
