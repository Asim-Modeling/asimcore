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

#ifndef _DRALDB_TRACKVEC_H
#define _DRALDB_TRACKVEC_H

#include "asim/TrackHeapDef.h"
#include "asim/TagIdVec.h"
#include "asim/ZipObject.h"

/**
  * @brief
  * This class tracks all the nodes and items requested.
  *
  * @description
  * All the elements of the database that the client want to
  * be tracked are stored in this vector. Each element has another
  * vector with the tracking of all of each tags.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class TrackIDNode : public ZipObject
{
    public:
        TrackIDNode();
        virtual ~TrackIDNode();

        // ---- ZibObject Interface methods
        ZipObject* compressYourSelf(INT32 cycle, bool last=false);
        // -----------------------------------------------

        void setType_MoveItem(UINT16 edge_id);
        void setType_NodeTag(UINT16 node_id, NodeSlot slot);
        void setType_EnterNode(UINT16 node_id, NodeSlot slot);
        void setType_ExitNode(UINT16 node_id, NodeSlot slot);
        void setType_CycleTag();

        inline TrackIdType getType();

        inline bool getTagValue(UINT16 tagId,INT32 cycle, UINT64*  value, UINT32* atcycle);
        inline bool getTagValue(UINT16 tagId,INT32 cycle, QString* value, UINT32* atcycle);
        inline bool getTagValue(UINT16 tagId,INT32 cycle, SOVList** value, UINT32* atcycle);

        inline bool addTagValue(UINT16 tagId,INT32 cycle, UINT64   value);
        inline bool addTagValue(UINT16 tagId,INT32 cycle, QString  value);
        inline bool addTagValue(UINT16 tagId,INT32 cycle, SOVList* value);

        inline void incPendingCnt(UINT16 tagId, INT32 cycle);
        inline void decPendingCnt(UINT16 tagId, INT32 cycle);

        TagIDList* getKnownTagIDs();

        void dumpTrackId();
        void dumpRegression();

    protected:
        void dumpTrackId_MoveItem();
        void dumpTrackId_NodeTag();
        void dumpTrackId_ExitNode();
        void dumpTrackId_EnterNode();
        void dumpTrackId_CycleTag();

        inline void checkTgId(UINT16 tagId);

    private:
        INT32 minTgId; // Minimum tag id tracked.
        INT32 maxTgId; // Maximum tag id tracked.
        TagIdVector tgIdVector; // Array to do the tracking of all the tags.
        TrackIdType type; // Which type of tracking is doing.
        TrackIdInfo trackSpec; // What is tracking this entry of the track vector.
};

/** @typedef TrackIDVector
  * @brief
  * Array that holds all the elements that are tracked in the
  * database. A maximum of 256k elements allowed.
  */
typedef AEVector<TrackIDNode,1024,256> TrackIDVector;

/**
 * Gets the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is found.
 */
bool
TrackIDNode::getTagValue(UINT16 tagId,INT32 cycle, UINT64*  value, UINT32* atcycle)
{
    // check if tagId is physically allocated
    if (! tgIdVector.hasElement(tagId) )
    {
        return false;
    }
    return tgIdVector[tagId].getTagValue(cycle,value,atcycle);
}

/**
 * Gets the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is found.
 */
bool
TrackIDNode::getTagValue(UINT16 tagId,INT32 cycle, QString* value, UINT32* atcycle)
{
    // check if tagId is physically allocated
    if (! tgIdVector.hasElement(tagId) )
    {
        return false;
    }
    return tgIdVector[tagId].getTagValue(cycle,value,atcycle);
}

/**
 * Gets the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is found.
 */
bool
TrackIDNode::getTagValue(UINT16 tagId,INT32 cycle, SOVList** value, UINT32* atcycle)
{
    // check if tagId is physically allocated
    if (! tgIdVector.hasElement(tagId) )
    {
        return false;
    }
    return tgIdVector[tagId].getTagValue(cycle,value,atcycle);
}

/**
 * Adds the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is added correctly.
 */
bool
TrackIDNode::addTagValue(UINT16 tagId,INT32 cycle, UINT64 value)
{
    minTgId = qMin(minTgId,(INT32)tagId);
    maxTgId = qMax(maxTgId,(INT32)tagId);
    checkTgId(tagId);
    return tgIdVector[tagId].addTagValue(cycle,value);
}

/**
 * Adds the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is added correctly.
 */
bool
TrackIDNode::addTagValue(UINT16 tagId,INT32 cycle, QString value)
{
    minTgId = qMin(minTgId,(INT32)tagId);
    maxTgId = qMax(maxTgId,(INT32)tagId);
    checkTgId(tagId);
    return tgIdVector[tagId].addTagValue(cycle,value);
}

/**
 * Adds the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is added correctly.
 */
bool
TrackIDNode::addTagValue(UINT16 tagId,INT32 cycle, SOVList* value)
{
    minTgId = qMin(minTgId,(INT32)tagId);
    maxTgId = qMax(maxTgId,(INT32)tagId);
    checkTgId(tagId);
    return tgIdVector[tagId].addTagValue(cycle,value);
}

/**
 * Returns the type of tracking.
 *
 * @return the type.
 */
TrackIdType
TrackIDNode::getType()
{
    return type;
}

/**
 * Allocates space for the tag tagId and sets the correct type
 * of tracking.
 *
 * @return void.
 */
void
TrackIDNode::checkTgId(UINT16 tagId)
{
    // make sure this tagId is phisically allocated
    tgIdVector.allocateElement(tagId);

    // review how to remove this if
    if ( (type==TRACKIDTYPE_NODETAG) && (!tgIdVector[tagId].hasData()) )
    {
        tgIdVector[tagId].setFwd(true);
    }

    if ( (type==TRACKIDTYPE_MOVEITEM) && (!tgIdVector[tagId].hasData()) )
    {
        tgIdVector[tagId].setUseDictionary(false);
    }
}

/**
 * Increments the pending counter for the tag tagId.
 *
 * @return void.
 */
void
TrackIDNode::incPendingCnt(UINT16 tagId, INT32 cycle)
{
    checkTgId(tagId);
    tgIdVector[tagId].incPendingCnt(cycle);
}

/**
 * Decrements the pending counter for the tag tagId.
 *
 * @return void.
 */
void
TrackIDNode::decPendingCnt(UINT16 tagId, INT32 cycle)
{
    checkTgId(tagId);
    tgIdVector[tagId].decPendingCnt(cycle); 
}

#endif
