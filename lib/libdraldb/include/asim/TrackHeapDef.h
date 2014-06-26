/* 
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

#ifndef _DRALDB_TRACKHEAPDEF_H_
#define _DRALDB_TRACKHEAPDEF_H_

#include "asim/draldb_syntax.h"
#include "asim/AEVector.h"
#include "asim/TagVec.h"
#include "asim/DRALTag.h"

/** @typedef enum TrackIdType
  * @brief
  * Enumeration of the different types of track.
  */
typedef enum
{
    TRACKIDTYPE_MOVEITEM = 0,
    TRACKIDTYPE_NODETAG,
    TRACKIDTYPE_CYCLETAG,
    TRACKIDTYPE_ENTERNODE,
    TRACKIDTYPE_EXITNODE
} TrackIdType;

/** @class TrackIdInfo
  * @brief
  * Holds the information to track the information of the different
  * types of track : move item and node tag.
  */
class TrackIdInfo
{
    public:
        UINT16   node_edge_id; // Id of the track.
        NodeSlot slot; // Node slot of the track.
} ;

/** @typedef TagIDList
  * @brief
  * Dynamic vector of tag id's.
  */
typedef AEVector<UINT16,32,128> TagIDList;

#endif
