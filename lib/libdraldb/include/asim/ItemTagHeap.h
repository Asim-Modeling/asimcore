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

#ifndef _DRALDB_ITEMTAGHEAP_H
#define _DRALDB_ITEMTAGHEAP_H

// Qt Library
#include <q3valuelist.h>
#include <qregexp.h>
#include <q3intcache.h>

#include "asim/draldb_syntax.h"
#include "asim/DralDBDefinitions.h"
#include "asim/AMemObj.h"
#include "asim/StatObj.h"
#include "asim/DRALTag.h"
#include "asim/ItemTagDef.h"
#include "asim/TagDescVector.h"
#include "asim/AEVector.h"
#include "asim/Dict2064.h"
#include "asim/DBConfig.h"
#include "asim/LogMgr.h"

//extern bool debug_on;

/*
 * Forward class declaration.
 */
class ItemHandler;
class TrackHeap;

/**
  * @brief
  * This class hold the heap of tags (tag-value) pairs.
  *
  * @description
  * This class holds all the values of all the tags of the items.
  * Each item has all its tags and values in consecutive positions.
  * If a tag is not mutable, just occupies one slot, but if its
  * value is changed along the simulation, each value change occupies
  * a heap slot. This class provides instructions to access and set
  * this information.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class ItemTagHeap : public AMemObj, public StatObj
{
    public:

        // ---- AMemObj Interface methods
        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------

    public:
        // -----------------------------------------------
        // Add Methods
        // -----------------------------------------------
        inline INT32 newItemBlock(INT32 itemId);

        void newTag(INT32 item_id, UINT16 tag_id, UINT64 value, UINT32 cycle);
        void newTag(INT32 item_id, UINT16 tag_id, SOVList * list, UINT32 cycle);

        // -----------------------------------------------
        // Consult Methods
        // -----------------------------------------------
        bool lookForItemId(ItemHandler * handler, INT32 itemid);

        bool getFirstItem(ItemHandler * handler);
        bool getLastItem(ItemHandler * handler);

        /**
          * Look for a given value in a given context (tag) starting from a given point.
          * @param target_tagid the tag id we are looking for
          * @param target_value the tag value we are looking for
          * @param staring_point vector entry where to start the scanning
          * @return vector position or -1 if <tag-value> not found
          */
        void lookForIntegerValue(ItemHandler * handler, UINT16 target_tagid,
            UINT64 target_value, UINT32 cycle, INT32 start_item);

        void lookForStrValue(ItemHandler * handler, UINT16 target_tagid,
            QString target_value, bool csensitive, bool exactMatch, UINT32 cycle,
            INT32 start_item);

        void lookForStrValue(ItemHandler * handler, UINT16 target_tagid,
            QRegExp target_value, UINT32 cycle, INT32 start_item);

        void reset();

        inline INT32 getNumItems();
        inline void setDralVersion(UINT16 value);
        inline UINT16  getDralVersion();
        inline void setFirstEffectiveCycle(INT32 value);

        void dumpItemTagHeap();
        void dumpRegression();

    public:
        static ItemTagHeap* getInstance();
        static void destroy();

    protected:
        // this a singleton class so protect constructors
        ItemTagHeap();
        virtual ~ItemTagHeap();

    protected:

        // Only Itemandler and TrackHeap can use these functions.
        friend class ItemHandler;
        friend class TrackHeap;

        bool getTag(ItemHandler * handler, UINT64 * tgvalue, TagValueType *, INT16 * tgbase, UINT64 * tgwhen, UINT32 cycle, bool backpropagate);
        bool getTag(ItemHandler * handler, UINT16 * tgId, UINT64 * tgvalue, UINT32 cycle, bool backpropagate);
        bool fastGetTag(ItemHandler * handler, UINT64* tgvalue, UINT32 cycle, bool backpropagate);

        bool getTagByName(ItemHandler * handler, QString tag, UINT64 * tgvalue, TagValueType * tgvtype, INT16 * tgbase, UINT64 * when, UINT32 cycle, bool backpropagate);
        bool getTagById(ItemHandler * handler, UINT16 rcvTgId, UINT64* tgvalue, TagValueType * tgvtype, INT16 * tgbase, UINT64 * tgwhen,UINT32 cycle, bool backpropagate);
        bool getTagPosition(ItemHandler * handler, UINT16 rcvTgId);

        /** Look for Special ItemId Tag in the given entry point
            @param heapIdx starting point of an item in the heap
            @return ItemId or -1 if invalid starting point
        */
        INT32 getItemId(ItemHandler * handler);

        bool skipToNextItem(ItemHandler * handler);
        bool skipToNextTag(ItemHandler * handler);
        bool skipToNextEntry(ItemHandler * handler);

        void rewindToFirstTagValue(ItemHandler * handler);

        // low level direct access methods
        UINT32 getCycle(ItemHandler * handler);
        UINT64 getValue(ItemHandler * handler);
        UINT16 getTagId(ItemHandler * handler);
        bool isLast(ItemHandler * handler);
        bool isSOV(ItemHandler * handler);
        bool isMutable(ItemHandler * handler);

        bool getMutableTagValue(ItemHandler * handler, UINT32 cycle, UINT64 * result, UINT64 * when, bool backpropagate);

        TagHeapNode * allocateNewSlot(ItemHandler * handler);
        TagHeapNode * shiftSlot(ItemHandler * handler);
        void resetTagState(ItemHandler * handler);

    private:
        INT32 firstEffectiveCycle;       // First cycle.
        INT32 nextItemVectorEntry;       // Next free item vector entry and the number of items.
        INT32 nextTagVectorEntry;        // Next free tag chunk entry.
        UINT16 dralVersion;              // Dral Version. Used for backwards compatibility.
        UINT16 canonicalItemId;          // Value of tag id that represents an ITEMID tag.
        ItemHeapVector * itemVector;     // Vector of item entries.
        TagHeapVector * tagVector;       // Vector of tag chunks.
        Q3IntCache<UINT32> * lookUpCache; // Cache to fasten up the item searches.
        TagDescVector * tdv;             // Pointer to the tag descriptor vector.
        Dict2064 * dict;                 // Pointer to a dictionary.
        DBConfig * conf;                 // Pointer to the database configuration state.
        LogMgr * myLogMgr;               // Pointer to the log manager.

    private:
        static ItemTagHeap* _myInstance; // Pointer to the singleton instance.
};

/**
 * This function inserts the itemId in the heap. The mapping
 * between the position and itemId is inserted in the lookUpCache
 * too.
 *
 * @return void.
 */
INT32
ItemTagHeap::newItemBlock(INT32 itemId)
{
    if(nextItemVectorEntry == MAX_ITEM_ENTRIES)
    {
        return -1;
    }

    //printf("NewItemBlock : %i, slot : %u\n", itemId, nextItemVectorEntry);

    // This allocates the position.
    (* itemVector)[nextItemVectorEntry].itemId = itemId;
    itemVector->ref(nextItemVectorEntry).first_chunk = 0;

    // Inserts the mapping to the cache.
    lookUpCache->insert((long) itemId, new UINT32(nextItemVectorEntry));
    // Occupies the position.
    return nextItemVectorEntry++;
}

/**
 * Returns the number of items inserted in the heap.
 *
 * @return the number of items.
 */
INT32
ItemTagHeap::getNumItems()
{
    return nextItemVectorEntry;
}

/**
 * Sets the dral version.
 *
 * @return void.
 */
void
ItemTagHeap::setDralVersion(UINT16 value)
{
    dralVersion = value;
}

/**
 * Returns the dral version.
 *
 * @return dral version.
 */
UINT16
ItemTagHeap::getDralVersion()
{
    return dralVersion;
}

/**
 * Sets the first effective cycle.
 *
 * @return void.
 */
void
ItemTagHeap::setFirstEffectiveCycle(INT32 value)
{
    firstEffectiveCycle = value;
}

#endif
