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
 * @file dralClientBinary_v4.cpp
 * @author Pau Cabre
 * @brief dral client binary version 1 implementation
 */

#include "asim/dralClientBinary_v4.h"

/**
 * Constructor of the ascii specific implementation of the dral client
 * it only invokes the generic implementation constructor
 */
DRAL_CLIENT_BINARY_4_IMPLEMENTATION_CLASS::DRAL_CLIENT_BINARY_4_IMPLEMENTATION_CLASS(
    DRAL_BUFFERED_READ dral_read,
    DRAL_LISTENER listener)
    : DRAL_CLIENT_BINARY_3_IMPLEMENTATION_CLASS (dral_read,listener)
{
}

DRAL_CLIENT_BINARY_4_IMPLEMENTATION_CLASS::~DRAL_CLIENT_BINARY_4_IMPLEMENTATION_CLASS()
{
}

bool
DRAL_CLIENT_BINARY_4_IMPLEMENTATION_CLASS::Comment()
{
    char * buffer2;
    UINT32 magic_num;
    UINT32 length;

    struct commentFormat
    {
        UINT8 commandCode  : 6;
        UINT8 reserved     : 2;
        UINT32 strlen;
        UINT32 magic_num;
    } * commandComment;

    commandComment = (commentFormat *) ((char *) ReadBytes(sizeof(commentFormat) - 1) - 1);

    if(EOS)
    {
        return false;
    }

    if (commandComment->strlen == 0)
    {
        dralListener->NonCriticalError(
            "Found a comment command with comment length == 0");
        return true;
    }

    magic_num=commandComment->magic_num;
    length=commandComment->strlen;
    buffer2 = (char *) ReadBytes(commandComment->strlen);
    if(EOS)
    {
        return false;
    }

    dralListener->Comment(magic_num,buffer2);

    return true;
}

bool
DRAL_CLIENT_BINARY_4_IMPLEMENTATION_CLASS::NewStringValue()
{
    struct newStringEntryFormat
    {
        UINT8 commandCode : 6;
        UINT8 reserved    : 2;
        UINT16 str_len;
        UINT16 str_id;
    } * stringEntry;

    stringEntry = (newStringEntryFormat *) ((char *) ReadBytes(sizeof(newStringEntryFormat) - 1) - 1);

    if(EOS)
    {
        return false;
    }

    UINT16 str_len = stringEntry->str_len;
    UINT16 str_id = stringEntry->str_id;

    const char * str = (const char *) ReadBytes(str_len);

    if(EOS)
    {
        return false;
    }

    strings[str_id] = getStringIndex(str, str_len);

    return true;
}
