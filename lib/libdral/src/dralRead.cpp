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
 * @file dralRead.cpp
 * @author Pau Cabre
 * @brief
 */

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "asim/dralRead.h"

#include <stdio.h>
#include <iostream>
using namespace std;

DRAL_BUFFERED_READ_CLASS::DRAL_BUFFERED_READ_CLASS (
    int file_descriptor, DRAL_LISTENER dral_listener, UINT32 buffer_size)
{
    fd=file_descriptor;
    numBytesRead=0;    
    bufferSize=buffer_size;
    dralListener=dral_listener;
    errorFound=false;

    if (bufferSize != 0)
    {
        buffer = (void *) new char [bufferSize + 1];
        if (buffer == NULL)
        {
            dralListener->Error("Not enought memory to allocate the buffer.");
            dralListener->EndSimulation();
            errorFound=true;
        }
        else
        {
            available = 0;
            pos = 0;
            file=gzdopen(dup(fd),"r");
            if (file == NULL)
            {
                dralListener->Error(strerror(errno));
                dralListener->EndSimulation();
                errorFound=true;
            }
        }
        buffer = (void *) (((char *) buffer) + 1);
    }
    else
    {
        dralListener->Error(
            "No valid buffer size");
        dralListener->EndSimulation();
        errorFound=true;
    }
}

DRAL_BUFFERED_READ_CLASS::~DRAL_BUFFERED_READ_CLASS ()
{
    if (buffer != NULL)
    {
        buffer = (void *) (((char *) buffer) - 1);
        delete [] (char *)buffer;
    }
    
    if (file != NULL)
    {
        gzclose(file);
    }
}


INT64 DRAL_BUFFERED_READ_CLASS::ReadFD(void * buf, UINT32 n)
{
    UINT32 i=0;
    INT64 r=0;

    while (r <= (n-i))
    {
        r=gzread(file,(char *)buf+i,n-i);

        if (r == -1)
        {
            int err;
            dralListener->Error((char *)gzerror(file,&err));
            if (err == Z_ERRNO)
            {
                dralListener->Error(strerror(errno));
            }
            return r;
        }
        else if (r == 0)
        {
            return i;
        }
        else
        {
            i+=r;
        }
    }
    return i;
}



void * DRAL_BUFFERED_READ_CLASS::Read(UINT32 n, INT64 * r)
{
    // n must be != 0
    INT64 tmp;
    
    if (errorFound)
    {
        *r=0;
        return NULL;
    }

    if (n > bufferSize)
    {
        //We want more bytes than the buffer size. That's not possible with
        //the current size, so we change the buffer size to n
        void * buffer_tmp = (void *) new char [n + 1];
        if (buffer_tmp == NULL)
        {
            dralListener->Error(
                "The buffer has been resized and we have run out of memory"
                " in that operation");
            dralListener->EndSimulation();
            errorFound=true;
            *r=0;
            return NULL;
        }
        memcpy(buffer_tmp,(char *)buffer+pos-1,available+1);
        pos=0;
        bufferSize=n;
        delete [] (((char *) buffer) - 1);
        buffer = (void *) (((char *) buffer_tmp) + 1);
    }

    if (n <= available)
    {
        pos+=n;
        available-=n;
        *r=n;
        numBytesRead+=n;
        return ((char *)buffer+pos-n);
    }
    else
    {
        memcpy((char *) buffer - 1,(char *) buffer + pos - 1, available + 1);
        tmp=ReadFD((void *)((char *)buffer+available),bufferSize-available);
        if (tmp == -1 || tmp == 0)
        {
            *r=tmp;
            return NULL;
        }
        pos=n;
        if ((available+tmp) < n)  // we wanted more bytes than the file has
        {
            *r=0;
            dralListener->Error(
                "Unexpected end of file\n"
                "Please report this bug to dral@bssad.intel.com attaching "
                "input files (if any)"
                );
            errorFound=true;
            return NULL;
        }
        available=available+tmp-n;
        *r=(INT64)n;
        numBytesRead+=n;
        return (buffer);
    }
}


INT64  /* It will return the file size if known or -1 otherwise */
DRAL_BUFFERED_READ_CLASS::GetFileSize (void)
{
    bool compressed = false;
    INT64 n;
    UINT32 size;
    
    INT64 current_byte = lseek(fd,0,SEEK_CUR);
    if (current_byte != -1)
    {
        if (lseek(fd,0,SEEK_SET) == -1)
        {
            dralListener->NonCriticalError(strerror(errno));
            return (-1);
        }
        else
        {
            unsigned char begining [2];
            n=read(fd,begining,2);
            if (n!=2)
            {
                dralListener->Error(strerror(errno));
                dralListener->EndSimulation();
                errorFound=true;
                return (-1);
            }
            if ((begining[0] == 0x1f) && (begining[1] == 0x8b))
            {
                compressed=true;
            }
        }
        
        if (compressed)
        {
            if (lseek(fd,-4,SEEK_END) == -1)
            {
                dralListener->Error(strerror(errno));
                dralListener->EndSimulation();
                errorFound=true;
                return (-1);
            }
            n=read(fd,(void *)&size,4);
            if (n!=4)
            {
                dralListener->Error(strerror(errno));
                dralListener->EndSimulation();
                errorFound=true;
                return (-1);
            }
        }
        else
        {
            struct stat s;
            if (fstat(fd,&s))
            {
                dralListener->Error(strerror(errno));
                dralListener->EndSimulation();
                errorFound=true;
                return (-1);
            }
            size=s.st_size;
        }

        if (lseek(fd,current_byte,SEEK_SET)==-1)
        {
            dralListener->Error(strerror(errno));
            dralListener->EndSimulation();
            errorFound=true;
            return (-1);
        }
        
        return size;
    }
    else
    {
        dralListener->NonCriticalError(strerror(errno));
        return (-1);
    }
}

UINT64 DRAL_BUFFERED_READ_CLASS::GetNumBytesRead (void)
{
    return numBytesRead;
}

UINT32 DRAL_BUFFERED_READ_CLASS::AvailableBytes (void)
{
    return available;
}
