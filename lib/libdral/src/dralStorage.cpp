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

#include "asim/dralStorage.h"

void DRAL_STORAGE_CLASS::ChangeImplementation(DRAL_SERVER_IMPLEMENTATION impl)
{
    implementation=impl;
}


DRAL_STORAGE_CLASS::DRAL_STORAGE_CLASS(DRAL_SERVER_IMPLEMENTATION impl)
{
    implementation=impl;
}

DRAL_STORAGE_CLASS::~DRAL_STORAGE_CLASS(void)
{
    for(
        list<DRAL_COMMAND_STORAGE>::const_iterator i=allCommandsList.begin();
        i != allCommandsList.end();
        ++i)
    {
        delete (*i);
    }
    allCommandsList.clear();
    numInstanceList.clear();
    partialCommandList.clear();
}

void DRAL_STORAGE_CLASS::ResetPartialList(void)
{
    partialCommandList.clear();
}

void DRAL_STORAGE_CLASS::Store(DRAL_COMMAND_STORAGE command, bool bothLists)
{
    allCommandsList.push_back(command);
    if (bothLists)
    {
        partialCommandList.push_back(command);
    }
}

void DRAL_STORAGE_CLASS::Store(
    DRAL_NEWNODE_STORAGE newNodeCommand,bool bothLists)
{
    Store(dynamic_cast<DRAL_COMMAND_STORAGE>(newNodeCommand),bothLists);
    numInstanceList.push_front(newNodeCommand);
}

void DRAL_STORAGE_CLASS::DumpPartialList(void)
{
    for(
        list<DRAL_COMMAND_STORAGE>::const_iterator i=partialCommandList.begin();
        i != partialCommandList.end();
        ++i)
    {
        (*i)->Notify(implementation);
    }
}

void DRAL_STORAGE_CLASS::DumpAllCommandsList(void)
{
    for(
        list<DRAL_COMMAND_STORAGE>::const_iterator i=allCommandsList.begin();
        i != allCommandsList.end();
        ++i)
    {
        (*i)->Notify(implementation);
    }
}

UINT16
DRAL_STORAGE_CLASS::GetInstance (const char node_name [])
{
    for(
        list<DRAL_NEWNODE_STORAGE>::const_iterator i=numInstanceList.begin();
        i != numInstanceList.end();
        ++i)
    {
        if (!strcmp((*i)->GetNodeName(), node_name))
        {
            return ((*i)->GetInstance()+1);
        }
    }
    return 0;
}
