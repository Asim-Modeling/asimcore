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
 * @file dralClientImplementation.h
 * @author Pau Cabre
 * @brief dral client implementation interface
 */


#ifndef DRAL_CLIENT_IMPLEMENTATION_H
#define DRAL_CLIENT_IMPLEMENTATION_H

#include <sstream>
#include <string.h>
#include <assert.h>
#include "asim/dral_syntax.h"
#include "asim/dralRead.h"
#include "asim/dralStringMapping.h"

using namespace std;

/**
 * implementation class interface
 * The client may have different implementations, according
 * to the type of the data read (ASCII or BINARY)
 */
class DRAL_CLIENT_IMPLEMENTATION_CLASS
{
  public:
    DRAL_CLIENT_IMPLEMENTATION_CLASS(DRAL_BUFFERED_READ dral_read, DRAL_LISTENER dralListener);
    virtual ~DRAL_CLIENT_IMPLEMENTATION_CLASS();

    virtual UINT16 ProcessNextEvent (bool blocking, UINT16 num_events)=0;

    INT64 GetFileSize(void);
    
    UINT64 GetNumBytesRead(void);

  protected:

    /*
     * a pointer to the class that performs buffered read
     */
    DRAL_BUFFERED_READ dralRead;

    /*
     * pointer to the listener
     */
    DRAL_LISTENER dralListener;
    

    /**
     * boolean used to know if an error has been found
     */
    bool errorFound;

    /*
     * stores the tags.
     */
    DRAL_STRING_MAPPING_CLASS tag_map;

    /*
     * stores the strings.
     */
    DRAL_STRING_MAPPING_CLASS str_val_map;

    /*
     * functions that map between strings and indexes.
     */
    UINT32 getTagIndex(const char * tag, UINT16 tag_len);
    UINT32 getStringIndex(const char * str, UINT16 str_len);

};

typedef DRAL_CLIENT_IMPLEMENTATION_CLASS * DRAL_CLIENT_IMPLEMENTATION;


#endif /* DRAL_CLIENT_IMPLEMENTATION_H */
