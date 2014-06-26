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

#ifndef _DRALDB_TAGVECITEMIDX_H
#define _DRALDB_TAGVECITEMIDX_H

#include "asim/DralDBDefinitions.h"
#include "asim/TagVec.h"

// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning

// This class holds UINT31 values, not UINT64 nor QString nor SOVList
// this is only for ItemIdx saved on Enter/ExitNode and MoveItems commands.

/**
  * @brief
  * This class is a tag vector that stores index ids.
  *
  * @description
  * This implementation of the tag vector is used to store item
  * ids instead of values. An item id matches only if is requested
  * in the exact cycle that when it was set.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */

/**
  * @typedef TagVecItemIdxNode
  * @brief
  * Entries of a tag vector item index.
  */
typedef struct
{
    UINT32 value: 31; // Item id of the entry.
    UINT32 used :  1; // Used or not.
} TagVecItemIdxNode;

// fwd reference of dense versions
class TagVecDenseItemIdx;
class TagVecDenseShortItemIdx;

class TagVecItemIdx : public TagVec
{
    public:
        TagVecItemIdx(INT32 bcycle);
        virtual ~TagVecItemIdx();

        inline bool getTagValue(INT32 cycle, UINT64*  value, UINT32* atcycle);
        inline bool getTagValue(INT32 cycle, SOVList** value, UINT32* atcycle);

        inline bool addTagValue(INT32 cycle, UINT64   value);
        inline bool addTagValue(INT32 cycle, QString  value);
        inline bool addTagValue(INT32 cycle, SOVList* value);

        void dumpCycleVector();
        inline TagVecEncodingType getType();

        ZipObject* compressYourSelf(INT32 cycle, bool last=false);

        friend class TagVecDenseItemIdx;
        friend class TagVecDenseShortItemIdx;

    protected:
        TagVecItemIdxNode valvec[CYCLE_CHUNK_SIZE]; // fixed size 4096 entries...
        INT32 baseCycle; // Base cycle of the vector.
};

/**
 * Adds a new tag in the vector.
 *
 * @return true if everything is ok.
 */
bool
TagVecItemIdx::addTagValue(INT32 cycle, UINT64 value)
{
    //printf ("TagVecItemIdx::addTagValue(cycle=%d,value=%llu\n",cycle,value);fflush(stdout);

    INT32 offset = cycle - baseCycle;
    Q_ASSERT ((offset>=0)&&(offset<CYCLE_CHUNK_SIZE));
    valvec[offset].value = (UINT32) value;
    valvec[offset].used  = 1;
    return true;
}

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVecItemIdx::addTagValue(INT32 cycle, QString  value)
{
    return false;
}

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVecItemIdx::addTagValue(INT32 cycle, SOVList* value)
{
    return false;
}

/**
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecItemIdx::getType()
{
    return TVEType_ITEMIDX;
}

/**
 * Gets the value of this tag in the cycle cycle.
 *
 * @return true if the value has been found.
 */
bool
TagVecItemIdx::getTagValue(INT32 cycle, UINT64*  value, UINT32* atcycle)
{
    INT32 offset = cycle - baseCycle;
    Q_ASSERT ((offset>=0)&&(offset<CYCLE_CHUNK_SIZE));
    if (! valvec[offset].used )
    {
        return false;
    }
    *value = (UINT64)valvec[offset].value;
	if (atcycle!=NULL)
    {
        *atcycle = cycle;
    }
    return true;
}

/**
 * Gets the value of this tag in the cycle cycle.
 *
 * @return true if the value has been found.
 */
bool
TagVecItemIdx::getTagValue(INT32 cycle, SOVList** value, UINT32* atcycle)
{
    return false;
}

#endif
