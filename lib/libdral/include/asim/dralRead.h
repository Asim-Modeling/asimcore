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
 * @file dralRead.h
 * @author Pau Cabre 
 * @brief
 */


#ifndef DRAL_READ_H
#define DRAL_READ_H

#include "asim/dral_syntax.h"
#include <zlib.h>
#include <pthread.h>
#include "asim/dralListener.h"

/**
 * This class performs the buffered read to the dral client
 */
class DRAL_BUFFERED_READ_CLASS
{

  public:

    /**
     * Constructor with the file descriptor, the dral listener (to comunicate
     * errors) and the buffer size
     */
    DRAL_BUFFERED_READ_CLASS (
        int file_descriptor, DRAL_LISTENER dralListener,
        UINT32 buffer_size = 4096);
    
    ~DRAL_BUFFERED_READ_CLASS();
    
    /**
     * This is the public method used to read bytes from the buffer.
     * It returns a pointer to the data and in 'r' the number of bytes read.
     * 'r' will be == to num_bytes if everything goes right.
     * If there is an error, 'r' will be 0 (end of file found) of -1 (error
     * reading the file descriptor). Note: this class will use the dral listener
     * to comunicate the error.
     * The parameter num_bytes should be != 0 (if it is 0, a NULL pointer will
     * be returned)
     */
    void * Read (UINT32 num_bytes, INT64 * r);

    /**
     * Public method used to know the size of the file associated to the file
     * descriptor.
     * It will return -1 if the size is impossible to know.
     */
    INT64 GetFileSize (void);
    
    /**
     * Public method used to know the number of bytes passed to the dral client.
     */
    UINT64 GetNumBytesRead (void);


    /**
     * Public method used to know how many unread bytes are still in the buffer
     */
    UINT32 AvailableBytes(void);

  private:

    /**
     * Private method that performs the read form the file descriptor to the
     * buffer.
     */
    INT64 ReadFD (void * buf, UINT32 num_bytes);

    /*
     * Private variables
     */
    int fd;
    gzFile file;
    UINT32 bufferSize;
    UINT32 available;
    UINT32 pos;
    UINT64 numBytesRead;
    void * buffer;
    DRAL_LISTENER dralListener;
    bool errorFound;

};
typedef DRAL_BUFFERED_READ_CLASS * DRAL_BUFFERED_READ; 

#endif /* DRAL_READ_H */
