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
 * @brief buffered write implementation
 */


#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
using namespace std;

#include "asim/dralWrite.h"
#include "asim/dralServerDefines.h"

DRAL_BUFFERED_WRITE_CLASS::DRAL_BUFFERED_WRITE_CLASS (
    UINT16 buffer_size, bool compression)
{
    if (buffer_size != 0)
    {
        buf_size=buffer_size;
        buffer = new char [buf_size];
        DRAL_ASSERT(buffer!=NULL,"Not enought memory to allocate the buffer");
        pos = 0;
        available = buf_size;
        buffered = true;
    }
    else
    {
        buffered = false;
    }
    memset((void *)zeros,0,8);
    file = NULL;
    uncompressed_file = NULL;
    compress = compression;
}

DRAL_BUFFERED_WRITE_CLASS::~DRAL_BUFFERED_WRITE_CLASS ()
{
    if (compress && (file != NULL))
    {
        gzclose(file);
    }

    if (!compress && (uncompressed_file != NULL))
    {
        fclose(uncompressed_file);
    }

    if (buffered)
    {
        delete [] buffer;
    }
}

void DRAL_BUFFERED_WRITE_CLASS::SetFileDescriptor (int fd)
{
    if (compress)
    {
        if (file != NULL)
        {
            gzclose(file);
        }
        file = gzdopen(dup(fd),"wb");
        DRAL_ASSERT(file!=NULL, "Error opening the file descriptor");
    }
    else
    {
        if (uncompressed_file != NULL)
        {
            fclose(uncompressed_file);
        }
        uncompressed_file = fdopen(dup(fd),"wb");
        DRAL_ASSERT(uncompressed_file!=NULL,
            "Error opening the file descriptor");
    }
}

void DRAL_BUFFERED_WRITE_CLASS::Write (const void * buf, UINT32 n)
{
    if (n==0)
    {
        /* Zlib produce errors if one tries to write 0 bytes */
        return;
    }
    if (buffered)
    {
        if (available >= n)
        {
            memcpy(buffer+pos,buf,n);
            pos+=n;
            available-=n;
        }
        else
        {
            Flush();
            if (available >= n)
            {
                memcpy(buffer+pos,buf,n);
                pos+=n;
                available-=n;
            }
            else
            {
                WriteFD(buf,n);
            }
        }
    }
    else
    {
        WriteFD(buf,n);
    }
}

void DRAL_BUFFERED_WRITE_CLASS::WriteFD (const void * buf, UINT32 n)
{
    INT32 k;
    if (compress)
    {
        DRAL_ASSERT(file!=NULL,"The file descriptor has not been set");
        k=gzwrite(file,(void *)buf,n);
    }
    else
    {
        DRAL_ASSERT(
            uncompressed_file!=NULL,"The file descriptor has not been set");
         k=fwrite((void *)buf,1,n,uncompressed_file);
    }
    DRAL_ASSERT(
        UINT32(k)==n,"Error writing to the file descriptor: " << strerror(errno));
}

void DRAL_BUFFERED_WRITE_CLASS::Flush (void)
{
    if ((file != NULL || uncompressed_file != NULL) && buffered)
                                   //this method is invoked when destroying
                                   //the DRAL_SERVER, so we have to check
                                   //whether the file descriptor is open or
                                   //it is not
    {
        WriteFD(buffer,pos);
        available=buf_size;
        pos=0;
    }
}
