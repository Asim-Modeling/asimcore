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
 * @file dralClient.h
 * @author Pau Cabre 
 * @brief dral client interface
 */

/**
 * @example dralClientExample
 * This is an example of a simple DRAL client
 */


#ifndef _DRALCLIENT_H
#define _DRALCLIENT_H

#include "asim/dral_syntax.h"
#include "asim/dralListener.h"
#include "asim/dralClientAscii_v0_1.h"
#include "asim/dralClientBinary_v0_1.h"
#include "asim/dralClientBinary_v2.h"
#include "asim/dralClientBinary_v3.h"
#include "asim/dralClientBinary_v4.h"


#define DRAL_CLIENT_VERSION_MAJOR 4 ///< Interface version
#define DRAL_CLIENT_ASCII_VERSION_MAJOR 1 ///< DEPRECATED
                                           // It used to be the supported
                                           // ascii inteface
#define DRAL_CLIENT_VERSION_MINOR 0 ///< Interface implementation version


/**
 * @brief The Dral Client class
 *
 * The main function of this class is to get events from a file
 * descriptor and to call the methods of the listener
 * according to the events found.
 */
class DRAL_CLIENT_CLASS
{

  public:

    /**
     * @brief Constructor specifying a file descriptor
     *
     * Constructor with the file descriptor from where the events will be read,
     * the listener with the callbacks to invoke when an event is found,
     * and the size of the read buffer
     * @param file_descriptor The file descriptor
     * @param dral_listener The DRAL_LISTENER
     * @param buffer_size The buffer size in bytes
     */
    DRAL_CLIENT_CLASS(
        int file_descriptor,  DRAL_LISTENER dral_listener,
        UINT32 buffer_size = 4096);

    /**
     * @brief The destructor.
     *
     * Used to free memory.
     */
    ~DRAL_CLIENT_CLASS (void);

    /**
     * @brief Method to know the dral file size.
     * @return The dral file size in bytes or -1 if it is
     * impossible to know (ex. the file descriptor is a socket)
     */
    inline INT64 GetFileSize(void);

    /**
     * @brief Method used to know the number of bytes read
     * @return The number of the bytes already read from the
     * dral file.
     */
    inline UINT64 GetNumBytesRead(void);

    /**
     * @brief Process the DRAL events
     *
     * This method will read from the file descriptor in order to process n
     * events by calling their specific method of the listener.
     * @param blocking It makes the method either blocking or non-blocking
     * WARNING: now it is always blocking
     * @param num_events The number of events the user want to process
     * @return The number of events processed or -1 when error
     */
    inline INT32 ProcessNextEvent (bool blocking, UINT16 num_events);

  private:

    /**
     * pointer to a class with the specific implementation of the client,
     * acoording to the type of the data read (BINARY or ASCII)
     */
    DRAL_CLIENT_IMPLEMENTATION implementation;

    /**
     * private method to check the type of the dral file and the version,
     * acording to this type, a specific implementation class will be
     * generated
     */
    UINT16 CheckFileType(UINT16 * ver);

    /**
     * boolean used to know if there is any problem in the dral client.
     * if there is, all the methods will do nothing (they will not try to
     * read the file descriptor).
     */
    bool error;

    DRAL_BUFFERED_READ dralRead;

};
typedef DRAL_CLIENT_CLASS * DRAL_CLIENT; ///<Pointer type to a DRAL_CLIENT_CLASS


// ----------------------------------------------------------------------
// Inlined methods
// ----------------------------------------------------------------------

INT32
DRAL_CLIENT_CLASS::ProcessNextEvent (
    bool blocking, UINT16 num_events)
{
    if (!error)
    {
        return implementation->ProcessNextEvent(blocking,num_events);
    }
    else
    {
        return -1;
    }
}

INT64
DRAL_CLIENT_CLASS::GetFileSize(void)
{
    if (!error)
    {
        return implementation->GetFileSize();
    }
    else
    {
        return -1;
    }
}


UINT64
DRAL_CLIENT_CLASS::GetNumBytesRead(void)
{
    if (!error)
    {
        return implementation->GetNumBytesRead();
    }
    else
    {
        return 0;
    }
}


#endif /* _DRALCLIENT_H */
