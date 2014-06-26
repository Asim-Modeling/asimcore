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

#ifndef _DRALDB_TRACKHEAP_H
#define _DRALDB_TRACKHEAP_H

#include <q3dict.h>

#include "asim/draldb_syntax.h"
#include "asim/AMemObj.h"
#include "asim/StatObj.h"
#include "asim/DRALTag.h"
#include "asim/TrackHeapDef.h"
#include "asim/TrackVec.h"
#include "asim/DBGraph.h"
#include "asim/ZipObject.h"
#include "asim/TagDescVector.h"
#include "asim/ItemHandler.h"

/**
  * @brief
  * This class stores the tracking of all the elements.
  *
  * @description
  * When a client wants to track the data of a node or edge of a
  * dral graph, then this heap saves all the values of the tags
  * of this element. Then, the clients can request the data stored
  * to know the values of the tags in any cycle.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class TrackHeap : public AMemObj, public StatObj, public ZipObject
{
    public:
        // ---- AMemObj Interface methods
        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // ---- ZibObject Interface methods
        ZipObject* compressYourSelf(INT32 cycle, bool last=false);
        // -----------------------------------------------

    public:
        // ----- Singleton Methods
        static TrackHeap* getInstance();
        static void destroy();
        // -----------------------------------------------

    public:
        // ----- API Methods
        bool trackEnterNode(UINT16 node_id, INT32 *fid, INT32 *lid); // reserve input_bw trackIds
        bool trackExitNode (UINT16 node_id, INT32 *fid, INT32 *lid); // reserve output_bw trackIds
        bool trackNodeTags (UINT16 node_id, INT32 *fid, INT32 *lid); // reserve all slots..

        INT32 trackNodeTags (UINT16 node_id, NodeSlot nslot);
        INT32 trackEnterNode(UINT16 node_id, NodeSlot nslot);
        INT32 trackExitNode (UINT16 node_id, NodeSlot nslot);

        bool trackMoveItem (UINT16 edge_id);

        INT32 trackCycleTags(); // by default cycle tags are ignored...

        inline bool getTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, UINT64*  value, UINT32* atcycle);
        inline bool getTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, QString* value, UINT32* atcycle);
        inline bool getTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, SOVList** value, UINT32* atcycle);
        inline bool getFormatedTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, QString* fmtvalue, UINT32* atcycle);

        inline bool addTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, UINT64   value);
        inline bool addTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, QString  value);
        inline bool addTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, SOVList* value);

        // -- MoveItem Consult Methods
        inline void getMoveItem(ItemHandler * handler, UINT16 edgeid,INT32 cycle,UINT16 pos=0);
        inline bool addMoveItem(UINT16 edgeid,INT32 itemIdx, INT32 cycle,UINT16 pos=0);

        // aux
        inline INT32 resolveTrackIdForNode(UINT16 node_id,UINT16 level, const UINT32 list []);
        inline INT32 resolveTrackIdForEnterNode(UINT16 node_id,UINT16 level, const UINT32 list []);
        inline INT32 resolveTrackIdForExitNode(UINT16 node_id,UINT16 level, const UINT32 list []);
        inline INT32 resolveTrackIdEdge(UINT16 edge_id);
        inline INT32 resolveTrackIdForCycleTag();

		inline bool getItemInSlot(INT32 trackId, INT32 cycle, ItemHandler * handler);

		void reset();
        void dumpTrackHeap();
		void dumpRegression();

        inline void setFirstEffectiveCycle(INT32 value);
        inline void incPendingCnt(INT32 trackId,UINT16 tagId,INT32 cycle);
        inline void decPendingCnt(INT32 trackId,UINT16 tagId,INT32 cycle);
        
        inline TagIDList* getKnownTagIDs(INT32 trackId);
        // -----------------------------------------------

    protected:
        // this a singleton class so protect constructors
        TrackHeap();
        virtual ~TrackHeap();

        inline INT32 resolveTrackIdFor(UINT16 node_id,UINT16 level, const UINT32 list [], Q3Dict<INT32>* hash);
        
    private:
        TrackIDVector trackIDVector; // Vector with all the tracking.
        INT32 nextTrackID; // Position of the next track.
        Q3Dict<INT32>* trackedNodes; // To keep track of tracking on nodes.
        INT32 cycleTagTrackID; // The track id for cycle tags.
        Q3Dict<INT32>* trackedEnterNodes; // To keep track of tracking on enter nodes.
        Q3Dict<INT32>* trackedExitNodes; // To keep track of tracking on exit nodes
        TagDescVector* tagdescvec; // Pointer to the tag description pointer.
        INT32 edgeTrackTbl[65536]; // Translation between node id and track id.
        INT32 tgItemIdxID; // Tag index used with items.
        INT32 firstEffectiveCycle; // First cycle of the database.

    private:
        static TrackHeap* _myInstance; // Pointer to the instance of this class.
        static DBGraph*   dbgraph; // Pointer to the database graph.
};

/**
 * Gets the value of the tag tagId of the track trackId in the
 * cycle cycle. The call is forwarded to the correct trackIdNode.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, UINT64*  value, UINT32* atcycle)
{
    Q_ASSERT(trackId<nextTrackID);
    bool ok = trackIDVector[trackId].getTagValue(tagId,cycle-firstEffectiveCycle,value,atcycle);
	if (atcycle!=NULL)
    {
        *atcycle += firstEffectiveCycle;
    }
	return ok;
}

/**
 * Gets the value of the tag tagId of the track trackId in the
 * cycle cycle. The call is forwarded to the correct trackIdNode.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, QString* value, UINT32* atcycle)
{
    Q_ASSERT(trackId<nextTrackID);
    bool ok = trackIDVector[trackId].getTagValue(tagId,cycle-firstEffectiveCycle,value,atcycle);
	if (atcycle!=NULL)
    {
        *atcycle += firstEffectiveCycle;
    }
	return ok;
}

/**
 * Gets the value of the tag tagId of the track trackId in the
 * cycle cycle. The call is forwarded to the correct trackIdNode.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, SOVList** value, UINT32* atcycle)
{
    Q_ASSERT(trackId<nextTrackID);
    bool ok = trackIDVector[trackId].getTagValue(tagId,cycle-firstEffectiveCycle,value,atcycle);
	if (atcycle!=NULL)
    {
        *atcycle += firstEffectiveCycle;
    }
	return ok;
}

/**
 * Gets the formatted value of the tag tagId of the track trackId
 * in the cycle cycle. The call is forwarded to the correct
 * trackIdNode.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getFormatedTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, QString* fmtvalue, UINT32* atcycle)
{
    bool qok;
    UINT64 ivalue;

    TagValueType type = tagdescvec->getTagValueType(tagId);
    switch (type)
    {
        case TagIntegerValue:
        qok = getTagValue(trackId,tagId,cycle,&ivalue,atcycle);
        if (!qok)
        {
            return false;
        }
        *fmtvalue = tagdescvec->getFormatedTagValue(tagId,ivalue);
        return true;
        break;

        case TagStringValue:
        return getTagValue(trackId,tagId,cycle,fmtvalue,atcycle);
        break;

        case TagSetOfValues:
        //SOVList lvalue;
        //qok = getTagValue(trackId,tagId,cycle,lvalue);
        break;

        default:
        return false;
    }
    return false;
}

/**
 * Gets the track id for the cycle tags.
 *
 * @return the trackid for cycle tags.
 */
INT32
TrackHeap::resolveTrackIdForCycleTag()
{
    return cycleTagTrackID;
}

/**
 * Given a node id and a slot information returns the track id.
 *
 * @return the trackid.
 */
INT32
TrackHeap::resolveTrackIdForNode(UINT16 node_id, UINT16 level, const UINT32 list [])
{
    return resolveTrackIdFor(node_id,level,list,trackedNodes);
}

/**
 * Given a node id and a slot information returns the track id.
 *
 * @return the trackid.
 */
INT32
TrackHeap::resolveTrackIdForEnterNode(UINT16 node_id,UINT16 level, const UINT32 list [])
{
    return resolveTrackIdFor(node_id,level,list,trackedEnterNodes);
}

/**
 * Given a node id and a slot information returns the track id.
 *
 * @return the trackid.
 */
INT32
TrackHeap::resolveTrackIdForExitNode(UINT16 node_id,UINT16 level, const UINT32 list [])
{
    return resolveTrackIdFor(node_id,level,list,trackedExitNodes);
}

/**
 * Resolves the track id for the slot in the node in the hash.
 *
 * @return the trackid.
 */
INT32
TrackHeap::resolveTrackIdFor(UINT16 node_id, UINT16 level, const UINT32 list [], Q3Dict<INT32>* hash)
{
    // Checks if the node exists.
    DBGraphNode* node = dbgraph->getNode(node_id);
    if (node==NULL)
    {
        return -1;
    }
    // Gets the track for this node.
    QString nname = node->getName();
    QString snname = DBGraph::slotedNodeName(nname,level,list);
    //printf("TrackHeap::resolveTrackIdFor ## snname=%s\n",snname.latin1());
    INT32* ptrack = hash->find(snname);
    if (ptrack==NULL)
    {
        return -1;
    }
    return *ptrack;
}

/**
 * Resolves the trackId for a given edge id.
 *
 * @return the track id.
 */
INT32
TrackHeap::resolveTrackIdEdge(UINT16 edge_id)
{
    //DBGraphEdge* edge = dbgraph->getEdge(edge_id);
    //if (edge==NULL) return -1;
    return edgeTrackTbl[edge_id];
}

/**
 * Adds a value in the tag tagId of the track trackId.
 *
 * @return true if the value is be added.
 */
bool
TrackHeap::addTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, UINT64 value)
{
    Q_ASSERT(trackId<nextTrackID);
    return trackIDVector[trackId].addTagValue(tagId,cycle-firstEffectiveCycle,value);
    /*
    printf ("TrackHeap::addTagValue called on trackId=%d, tgId=%d,cycle=%d,value=%llu\n",
    trackId,tagId,cycle,value);
    */
}

/**
 * Adds a value in the tag tagId of the track trackId.
 *
 * @return true if the value is be added.
 */
bool
TrackHeap::addTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, QString value)
{
    Q_ASSERT(trackId<nextTrackID);
    return trackIDVector[trackId].addTagValue(tagId,cycle-firstEffectiveCycle,value);
}

/**
 * Adds a value in the tag tagId of the track trackId.
 *
 * @return true if the value is be added.
 */
bool
TrackHeap::addTagValue(INT32 trackId, UINT16 tagId,INT32 cycle, SOVList* value)
{
    return false;
}

// -------------------------------------------------------------------
// -- MoveItem Consult Methods
// -------------------------------------------------------------------
/**
 * Gets a move of in the position pos of the edge edgeid in the
 * cycle cycle.
 *
 * @return if a move item exist.
 */
void
TrackHeap::getMoveItem(ItemHandler * handler, UINT16 edgeid,INT32 cycle,UINT16 pos)
{
    //printf("TrackHeap::getMoveItem edgeid=%d,cycle=%d,pos=%d\n",(int)edgeid,(int)cycle,(int)pos);
    // Checks if the edge exists.
    DBGraphEdge* edge = dbgraph->getEdge(edgeid);
    if (edge==NULL)
    {
        // Just return an invalid handler.
		handler->invalid();
        return;
    }
    // check pos
    if (pos>=edge->getBandwidth())
    {
		handler->invalid();
        return;
    }

    UINT64 value;
    INT32  trackid = edgeTrackTbl[edgeid];
    //printf ("base track id = %d\n",(int)trackid);
    // check registration
    if (trackid<0)
    {
		handler->invalid();
        return;
    }

    // adjust by position
    trackid += pos;
    //printf ("bw-adjusted track id = %d\n",(int)trackid);

    bool gok = trackIDVector[trackid].getTagValue(tgItemIdxID,cycle-firstEffectiveCycle,&value,NULL);
    if (!gok)
    {
		handler->invalid();
        return;
    }
	handler->itemIdx = (INT32) value;
	handler->valid_item = true;
	ItemTagHeap::getInstance()->resetTagState(handler);
}

/**
 * Adds a move of the item itemIdx in the cycle cycle of the in
 * the position pos of the edge edgeid.
 *
 * @return if a move item is added.
 */
bool
TrackHeap::addMoveItem(UINT16 edgeid,INT32 itemIdx, INT32 cycle,UINT16 pos)
{
    /*
    printf ("TrackHeap::addMoveItem on edge=%d,itemIdx=%d,cycle=%d,pos=%d, fec=%d\n",
    (int)edgeid,itemIdx,cycle,pos,firstEffectiveCycle);
    */
    // Checks that the edge exists.
    DBGraphEdge* edge = dbgraph->getEdge(edgeid);
    if (edge==NULL)
    {
        return false;
    }

    // check pos
    if ((int)pos >= (int)(edge->getBandwidth()))
    {
        return false;
    }

    UINT64 value;
    INT32  trackid = edgeTrackTbl[edgeid];

    //printf ("TrackHeap::addMoveItem base-trackid=%d\n",trackid);

    // check registration
    if (trackid<0)
    {
        return false;
    }

    // adjust by position
    trackid += pos;
    //printf("bw-adj trackid =%d\n",trackid);

    return trackIDVector[trackid].addTagValue(tgItemIdxID,cycle-firstEffectiveCycle,(UINT64)itemIdx);
}

/**
 * Gets the item inside the slot of this track.
 *
 * @return void.
 */
bool
TrackHeap::getItemInSlot(INT32 trackId, INT32 cycle, ItemHandler * handler)
{
	UINT64 value;

    bool gok = trackIDVector[trackId].getTagValue(tgItemIdxID, cycle - firstEffectiveCycle, &value, NULL);
    if(!gok)
    {
		handler->invalid();
        return false;
    }
	handler->itemIdx = (INT32) value;
	handler->valid_item = true;
	ItemTagHeap::getInstance()->resetTagState(handler);
	return true;
}

/**
 * Sets the first effective cycle of the database.
 *
 * @return void.
 */
void
TrackHeap::setFirstEffectiveCycle(INT32 value)
{
    firstEffectiveCycle = value;
}

/**
 * Increments the pending counter of the tag tagId of the track
 * trackId.
 *
 * @return void.
 */
void
TrackHeap::incPendingCnt(INT32 trackId,UINT16 tagId,INT32 cycle)
{
    Q_ASSERT(trackId<nextTrackID);
    trackIDVector[trackId].incPendingCnt(tagId,cycle-firstEffectiveCycle);
}

/**
 * Decrements the pending counter of the tag tagId of the track
 * trackId.
 *
 * @return void.
 */
void
TrackHeap::decPendingCnt(INT32 trackId,UINT16 tagId,INT32 cycle)
{
    Q_ASSERT(trackId<nextTrackID);
    trackIDVector[trackId].decPendingCnt(tagId,cycle-firstEffectiveCycle);
}

/**
 * Gets the known tags of the track trackId.
 *
 * @return the tag name list.
 */
TagIDList*
TrackHeap::getKnownTagIDs(INT32 trackId)
{
    if (trackId>=nextTrackID)
    {
        return NULL;
    }

    return trackIDVector[trackId].getKnownTagIDs();
}

#endif
