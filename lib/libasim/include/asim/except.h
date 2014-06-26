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
 * @author David Goodwin
 * @brief Asynchronous exception delivery (multicast).
 */

#ifndef _EXCEPT_
#define _EXCEPT_

// generic
#include <string.h>

// ASIM core
#include "asim/syntax.h"
#include "asim/mesg.h"

#define MAX_EXCEPTIONS 	64

typedef class ASIM_MODULE_CLASS *ASIM_MODULE;
typedef class ASIM_EXCEPTDATA_CLASS *ASIM_EXCEPTDATA;


/*
 * ASIM_EXCEPTION
 *
 * Type for exceptions.
 */
typedef UINT32 ASIM_EXCEPTION;


/*
 * Class ASIM_EXCEPTDATA
 *
 * Exception data sent when a module is notified of
 * an exception.
 */

typedef class ASIM_EXCEPTDATA_CLASS *ASIM_EXCEPTDATA;
class ASIM_EXCEPTDATA_CLASS
{
    private:
        static UINT64 uniqueExceptId;
        
        const UINT64 uid;
        const ASIM_EXCEPTION type;

    public:
        ASIM_EXCEPTDATA_CLASS (const ASIM_EXCEPTION t) 
	    : uid(uniqueExceptId), type(t)
	{
            uniqueExceptId++;
        }

        virtual ~ASIM_EXCEPTDATA_CLASS () { }

        /*
         * Accessors...
         */
        UINT32 Type (void) const { return(type); }
        UINT64 Uid (void) const { return(uid); }

};



/*
 * Class ASIM_EXCEPT
 *
 * Exception handler. Relays exceptions to modules
 * that have registered to recieve those exceptions.
 *
 */
typedef class ASIM_EXCEPT_CLASS *ASIM_EXCEPT;
class ASIM_EXCEPT_CLASS
{
    public:
        /*
         * Register that module 'm' wants to be informed of
         * 'xcp' type exceptions.
         */
        virtual void Register (ASIM_MODULE m, ASIM_EXCEPTION xcp) =0;

        /*
         * Notify all modules of exception 'data'.
         */
        virtual void Raise (ASIM_EXCEPTDATA data, UINT64 cycle) =0;
            
};

#endif /* _EXCEPT_ */
