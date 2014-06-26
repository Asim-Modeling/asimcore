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

#ifndef _DRALDB_TAGVECDENSESHORTITEMIDX_H
#define _DRALDB_TAGVECDENSESHORTITEMIDX_H

#include "asim/DralDBDefinitions.h"
#include "asim/TagVecItemIdx.h"

/**
  * @brief
  * This class is a tag vector that stores index ids.
  *
  * @description
  * This implementation of the index tag vector just uses 20 bits
  * to store the item id and 12 for the offset. Only 20 bits are
  * needed because is guaranteed that the maximum value enter is
  * never greater than 2^20.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */

// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning
// Warning Warning Warning Warning Warning Warning Warning Warning Warning Warning

// This class holds UINT20 values, not UINT64 nor QString nor SOVList
// this is only for ItemIdx saved on Enter/ExitNode and MoveItems commands.

/**
  * @typedef TagVecDenseShortItemIdxNode
  * @brief
  * Entries of a tag vector item index.
  */
typedef struct
{
    UINT32 value   : 20; // Item id of the entry.
    UINT32 offset  : 12; // Offset of the entry
} TagVecDenseShortItemIdxNode;

class TagVecDenseShortItemIdx : public TagVec
{
    public:
        TagVecDenseShortItemIdx(TagVecItemIdx* source);
        virtual ~TagVecDenseShortItemIdx();

        inline bool getTagValue(INT32 cycle, UINT64*  value, UINT32* atcycle);
        inline bool getTagValue(INT32 cycle, SOVList** value, UINT32* atcycle);

        inline bool addTagValue(INT32 cycle, UINT64   value);
        inline bool addTagValue(INT32 cycle, QString  value);
        inline bool addTagValue(INT32 cycle, SOVList* value);

        void dumpCycleVector();
        inline TagVecEncodingType getType();

        ZipObject* compressYourSelf(INT32 cycle, bool last=false);

    protected:
        TagVecDenseShortItemIdxNode* valvec; // Array of indexes.
        INT32 nextEntry; // Position of the next entry.
        INT32 baseCycle; // Base cycle of the vector.
};

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVecDenseShortItemIdx::addTagValue(INT32 cycle, UINT64 value)
{
    return false;
}

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVecDenseShortItemIdx::addTagValue(INT32 cycle, QString  value)
{
    return false;
}

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVecDenseShortItemIdx::addTagValue(INT32 cycle, SOVList* value)
{
    return false;
}

/**
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecDenseShortItemIdx::getType()
{
    return TVEType_DENSE_ITEMIDX;
}

/**
 * Gets the value (item id) in the cycle cycle.
 *
 * @return true if the value has been found.
 */
bool
TagVecDenseShortItemIdx::getTagValue(INT32 cycle, UINT64* value, UINT32* atcycle)
{
    INT32 i=0;
    bool fnd = false ;
	UINT32 ccycle = 0;
    while (!fnd && ((valvec[i].offset+baseCycle)<=(UINT32)cycle) && (i<nextEntry))
    {
		ccycle = valvec[i].offset+baseCycle;
        fnd = ccycle == (UINT32)cycle ;
        *value = (UINT64)(valvec[i].value);
        ++i;
    }
	if (fnd && (atcycle!=NULL) )
    {
        *atcycle = ccycle;
    }
    return fnd;
}

/**
 * Gets the value of this tag in the cycle cycle.
 *
 * @return always false.
 */
bool
TagVecDenseShortItemIdx::getTagValue(INT32 cycle, SOVList** value, UINT32* atcycle)
{
    return false;
}

#endif
