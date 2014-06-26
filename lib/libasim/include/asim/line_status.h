/*****************************************************************************
 *
 * @brief Header file for Line Status
 *
 * @author Oscar Rosell created the file from a part of cache.h
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

#ifndef LINE_STATUS_H
#define LINE_STATUS_H

//
// Major Cache Line Coherency States
//
typedef enum
{ 
    S_PERFECT,
    S_WARM,
    S_MODIFIED, 
    S_EXCLUSIVE, 
    S_SHARED, 
    S_INVALID, 
    S_FORWARD, 
    S_LOCKED,
    S_RESERVED,
    S_NC_CLEAN,
    S_NC_DIRTY,
    S_MAX_LINE_STATUS 
} LINE_STATUS;

static const char * LINE_STATUS_STRINGS[S_MAX_LINE_STATUS] =
{ 
    "Perfect",
    "Warm",
    "Modified", 
    "Exclusive", 
    "Shared", 
    "Invalid", 
    "Forward",
    "Locked",
    "Reserved",
    "NC_clean",
    "NC_dirty"
};

#endif
