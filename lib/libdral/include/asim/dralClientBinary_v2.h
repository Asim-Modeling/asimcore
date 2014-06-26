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
 * @file dralClientBinary_v2.h
 * @author Pau Cabre 
 * @brief dral client binary defines for dral version 2
 */

#ifndef DRAL_CLIENT_BINARY_V2_H
#define DRAL_CLIENT_BINARY_V2_H

#include "asim/dralClientImplementation.h"
#include "asim/dralClientDefines.h"     /* We need to include this file because
                                        there is the definition of the
                                        DRAL_VERSION command code */


#define DRAL2_CYCLE                      0
#define DRAL2_NEWITEM                    1
#define DRAL2_MOVEITEMS                  2
#define DRAL2_DELETEITEM                 3
#define DRAL2_SETITEMTAG                 4
#define DRAL2_SETITEMTAG_STRING          5
#define DRAL2_SETITEMTAG_SET             6
#define DRAL2_ENTERNODE                  7
#define DRAL2_EXITNODE                   8
#define DRAL2_NEWNODE                    9
#define DRAL2_NEWEDGE                    10
#define DRAL2_SETNODELAYOUT              11
#define DRAL2_COMMENT                    12
//13 used for DRAL_VERSION. Already defined
#define DRAL2_SETNODETAG                 14
#define DRAL2_SETNODETAG_STRING          15
#define DRAL2_SETNODETAG_SET             16
#define DRAL2_SETCYCLETAG                17
#define DRAL2_SETCYCLETAG_STRING         18
#define DRAL2_SETCYCLETAG_SET            19
#define DRAL2_SETNODEINPUTBANDWIDTH      20
#define DRAL2_SETNODEOUTPUTBANDWIDTH     21
#define DRAL2_STARTACTIVITY              22
#define DRAL2_SETTAGDESCRIPTION          23
#define DRAL2_COMMENTBIN                 24
#define DRAL2_SETNODECLOCK               25
#define DRAL2_NEWCLOCK                   26
#define DRAL2_CYCLEWITHCLOCK             27

/**
 * BINARY client version 2 implementation class
 */
class DRAL_CLIENT_BINARY_2_IMPLEMENTATION_CLASS
    : public DRAL_CLIENT_IMPLEMENTATION_CLASS
{
  public:

    DRAL_CLIENT_BINARY_2_IMPLEMENTATION_CLASS (
        DRAL_BUFFERED_READ dral_read, DRAL_LISTENER dralListener);
    
    UINT16 ProcessNextEvent (bool blocking, UINT16 num_events);

  protected:

    void * ReadBytes (UINT32 num_bytes);

    /**
     * Private methods to proccess theses specific events
     */
    bool Cycle (void * b);
    bool NewItem (void * b);
    bool DeleteItem (void * b);
    bool EnterNode (void * b);
    bool ExitNode (void * b);
    bool SetItemTag (void * b, UINT16 v);
    bool MoveItems (void * b);
    bool NewNode (void * b);
    bool NewEdge (void * b);
    bool SetNodeLayout (void * b);
    bool Comment (void * b);
    bool CommentBin (void * b);
    bool Error (void * b);
    bool SetNodeTag (void * b, UINT16 v);
    bool SetCycleTag (void * b, UINT16 v);
    bool SetNodeInputBandwidth (void * b);
    bool SetNodeOutputBandwidth (void * b);
    bool StartActivity (void * b);
    bool SetTagDescription (void * b);
    bool SetNodeClock (void * b);
    bool NewClock (void * b);
    bool CycleWithClock (void * b);

};


#endif /* DRAL_CLIENT_BINARY_V2_H */
