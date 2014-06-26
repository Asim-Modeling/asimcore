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
 * @file dralWrite.h
 * @author Pau Cabre 
 * @brief buffered write interface
 */


#ifndef DRAL_WRITE_H
#define DRAL_WRITE_H

#include <zlib.h>
#include <stdio.h>

#include "asim/dral_syntax.h"

/*
 * This class performs the buffered writing to the file descriptor
 */
class DRAL_BUFFERED_WRITE_CLASS
{

  public:

    DRAL_BUFFERED_WRITE_CLASS (UINT16 buffer_size, bool compression);
    
    void SetFileDescriptor (int fd);
    
    ~DRAL_BUFFERED_WRITE_CLASS(void);
    
    void Write (const void * buf, UINT32 num_bytes);
    
    void Flush (void);

  private:

    UINT16 buf_size;  // the buffer size
    
    char * buffer;
    
    bool buffered;
    
    UINT16 available;  // available bytes in the buffer
    
    UINT16 pos;  // position of the begining of the free area in the buffer
    
    /*
     * Private method that performs the writing of the buffer to the file
     * descriptor
     */
    void WriteFD (const void * buf, UINT32 num_bytes);

    char zeros [8];
    
    gzFile file;
    FILE * uncompressed_file;

    bool compress;
};
typedef DRAL_BUFFERED_WRITE_CLASS * DRAL_BUFFERED_WRITE;

#endif /* DRAL_WRITE_H */
