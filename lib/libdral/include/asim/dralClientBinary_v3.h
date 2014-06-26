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
 * @file dralClientBinary_v3.h
 * @author Pau Cabre 
 * @brief dral client binary defines for dral version 3
 */

#ifndef DRAL_CLIENT_BINARY_V3_H
#define DRAL_CLIENT_BINARY_V3_H

#include "asim/dralClientImplementation.h"
#include "asim/dralClientDefines.h"     /* We need to include this file because
                                        there is the definition of the
                                        DRAL_VERSION command code */

enum DRAL3_COMMANDS
{
    DRAL3_CYCLE,
    DRAL3_STARTACTIVITY,
    DRAL3_NEWNODE,
    DRAL3_NEWEDGE,
    DRAL3_SETNODELAYOUT,
    DRAL3_SETNODEINPUTBANDWIDTH,
    DRAL3_SETNODEOUTPUTBANDWIDTH,
    DRAL3_COMMENT,
    DRAL3_COMMENTBIN,
    DRAL3_SETNODECLOCK,
    DRAL3_NEWCLOCK,
    DRAL3_CYCLEWITHCLOCK,
    DRAL3_SETTAGDESCRIPTION,
    DRAL3_VERSION, ///< WARNING: must be 13!!!
    DRAL3_NEWITEM,
    DRAL3_DELETEITEM,
    DRAL3_MOVEITEMS,
    DRAL3_ENTERNODE,
    DRAL3_EXITNODE,
    DRAL3_SETITEMTAG_VALUE_8_BITS,
    DRAL3_SETITEMTAG_VALUE_16_BITS,
    DRAL3_SETITEMTAG_VALUE_32_BITS,
    DRAL3_SETITEMTAG_STRING,
    DRAL3_SETITEMTAG_SET,
    DRAL3_SETNODETAG_VALUE_WITHLEVELS,
    DRAL3_SETNODETAG_VALUE_WITHOUTLEVELS_8_BITS,
    DRAL3_SETNODETAG_VALUE_WITHOUTLEVELS_16_BITS,
    DRAL3_SETNODETAG_STRING,
    DRAL3_SETNODETAG_SET,
    DRAL3_SETCYCLETAG_VALUE,
    DRAL3_SETCYCLETAG_STRING,
    DRAL3_SETCYCLETAG_SET,
    DRAL3_NEWTAG,
    DRAL3_NEWSTRINGVALUE
} ;

enum DRAL3_VALUE_SIZE
{
    VALUE_8_BITS,
    VALUE_16_BITS,
    VALUE_32_BITS,
    VALUE_64_BITS
} ;

/**
 * BINARY client version 3 implementation class
 */
class DRAL_CLIENT_BINARY_3_IMPLEMENTATION_CLASS
    : public DRAL_CLIENT_IMPLEMENTATION_CLASS
{
  public:

    DRAL_CLIENT_BINARY_3_IMPLEMENTATION_CLASS (
        DRAL_BUFFERED_READ dral_read, DRAL_LISTENER dralListener);
    
    virtual ~DRAL_CLIENT_BINARY_3_IMPLEMENTATION_CLASS ();

    UINT16 ProcessNextEvent (bool blocking, UINT16 num_events);

  protected:

    bool EOS;

    UINT32 tags[256];
    UINT32 strings[65536];

    INT32 last_item;
    INT32 last_node;
    INT32 last_edge;

    void * ReadBytes (UINT32 num_bytes);
    UINT64 getValue(DRAL3_VALUE_SIZE val_size);
    INT32 getDelta(DRAL3_VALUE_SIZE item_size);

    /**
     * Private methods to proccess theses specific events
     */
    bool Error ();

    bool Cycle ();
    bool StartActivity ();
    bool NewNode ();
    bool NewEdge ();
    bool SetNodeLayout ();
    bool SetNodeInputBandwidth ();
    bool SetNodeOutputBandwidth ();
    virtual bool Comment ();
    bool CommentBin ();
    bool SetNodeClock ();
    bool NewClock ();
    bool CycleWithClock ();
    bool SetTagDescription ();
    bool NewItem (void * buffer);
    bool DeleteItem (void * buffer);
    bool MoveItems ();
    bool EnterNode ();
    bool ExitNode ();
    bool SetItemTagValue (DRAL3_VALUE_SIZE item_size);
    bool SetItemTagOther ();
    bool SetNodeTagValueNoLevels (DRAL3_VALUE_SIZE node_size);
    bool SetNodeTagValueLevels ();
    bool SetNodeTagOther ();
    bool SetCycleTagValue ();
    bool SetCycleTagOther ();
    bool NewTag ();
    virtual bool NewStringValue ();
};

#endif /* DRAL_CLIENT_BINARY_V3_H */
