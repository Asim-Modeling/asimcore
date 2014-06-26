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
 * @file dralClientImplementation.cpp
 * @author Pau Cabre
 * @brief dral client implemntation base
 */
 
#include "asim/dralClientImplementation.h"

/**
 * Constructor with the file descriptor from where the data will be read,
 * the listener and the read buffer size
 */
DRAL_CLIENT_IMPLEMENTATION_CLASS::DRAL_CLIENT_IMPLEMENTATION_CLASS (
    DRAL_BUFFERED_READ dral_read, DRAL_LISTENER listener)
// Warning!!!
// This values aren't a bug. We need to assign big maximum values to this
// mappings.
    : tag_map(1024 * 1024 * 1024), str_val_map(1024 * 1024 * 1024)
{
    dralListener = listener;
    errorFound = false;
    dralRead = dral_read;
}

DRAL_CLIENT_IMPLEMENTATION_CLASS::~DRAL_CLIENT_IMPLEMENTATION_CLASS()
{
}

INT64
DRAL_CLIENT_IMPLEMENTATION_CLASS::GetFileSize(void)
{
    return dralRead->GetFileSize();
}

UINT64
DRAL_CLIENT_IMPLEMENTATION_CLASS::GetNumBytesRead(void)
{
    return dralRead->GetNumBytesRead();
}

UINT32
DRAL_CLIENT_IMPLEMENTATION_CLASS::getTagIndex(const char * tag, UINT16 tag_len)
{
    UINT32 index;

    if(tag_map.getMapping(tag, tag_len, &index))
    {
        dralListener->NewTag(index, tag, tag_len);
    }

    return index;
}

UINT32
DRAL_CLIENT_IMPLEMENTATION_CLASS::getStringIndex(const char * str, UINT16 str_len)
{
    UINT32 index;

    if(str_val_map.getMapping(str, str_len, &index))
    {
        dralListener->NewString(index, str, str_len);
    }

    return index;
}
