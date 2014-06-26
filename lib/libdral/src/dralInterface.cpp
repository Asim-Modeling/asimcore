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
 * @file dralInterface.cpp
 * @author Julio Gago
 * @brief dral low-level-buy-pretty interface
 *
 * Please refer to header file for comments and usage.
 *
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <iostream>
using namespace std;

#include "asim/dralInterface.h"

UINT32 DRAL_ITEM::current_id = 1;
UINT32 DRAL_NODE::current_id = 1;
UINT32 DRAL_EDGE::current_id = 1;

/*
 * A default DRAL server. Used when the user does not explicitly create and
 * provide one or more (most of the times).
 */
DRAL_SERVER_CLASS defaultDralServer("events.drl", 4096, false, true, false);

/*
 * Several shortcuts to access the default DRAL server
 *
 */
void DralStartActivity()
{
    defaultDralServer.StartActivity();
}

void DralTurnOn(void)
{
    defaultDralServer.TurnOn();
}

void DralCycle(UINT64 n)
{
    defaultDralServer.Cycle(n);
}

void DralTurnOff(void)
{
    defaultDralServer.TurnOff();
}

/*
 * Syntax sugar to create name/value TAG pairs using a single and compact
 * function name, but without the use of a DRAL_TAG objects, which is in any
 * case recommended :-).
 */
DRAL_uTAG dTag(const char tag_name[], UINT64 value)       { return *(new DRAL_uTAG(tag_name, value)); }
DRAL_uTAG dTag(const char tag_name[], INT64 value)        { return *(new DRAL_uTAG(tag_name, (UINT64) value)); } 
DRAL_uTAG dTag(const char tag_name[], UINT32 value)       { return *(new DRAL_uTAG(tag_name, (UINT64) value)); }
DRAL_uTAG dTag(const char tag_name[], INT32 value)        { return *(new DRAL_uTAG(tag_name, (UINT64) value)); }
DRAL_cTAG dTag(const char tag_name[], char value)         { return *(new DRAL_cTAG(tag_name, value)); }
DRAL_sTAG dTag(const char tag_name[], const char value[]) { return *(new DRAL_sTAG(tag_name, value)); }

