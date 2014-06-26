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

#ifndef _DRALDB_TAGVECDICTIONARY_H
#define _DRALDB_TAGVECDICTIONARY_H

#include "asim/DralDBDefinitions.h"
#include "asim/TagVec.h"
#include "asim/StrTable.h"
#include "asim/Dict2064.h"

#define TAGVECDICTIONARY_MIN_DENSITY 0.75

/**
  * @brief
  * This class is a tag vector that uses a dictionary.
  *
  * @description
  * This implementation uses the Dict2064 class to map the keys
  * and values of the entries. The information is not compressed
  * in this class.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */

/**
  * @typedef TagVecDictionaryNode
  * @brief
  * Entries of a tag vector dictionary.
  */
typedef struct
{
    UINT32 cycleOffset: 12; // Offset with the base cycle.
    UINT32 key :        20; // Key used to know the value of the entry using the Dict2064.
} TagVecDictionaryNode;

// fwd reference of dense version
class TagVecDenseDictionary;

class TagVecDictionary : public TagVec
{
    public:
        TagVecDictionary(INT32 bcycle);
        virtual ~TagVecDictionary();

        inline  bool getTagValue(INT32 cycle, UINT64*  value, UINT32* atcycle);
        inline  bool getTagValue(INT32 cycle, SOVList** value, UINT32* atcycle);

        inline bool addTagValue(INT32 cycle, UINT64   value);
        inline bool addTagValue(INT32 cycle, QString  value);
        inline bool addTagValue(INT32 cycle, SOVList* value);

        void dumpCycleVector();
        inline TagVecEncodingType getType();

        ZipObject* compressYourSelf(INT32 cycle, bool last=false);

        friend class TagVecDenseDictionary;

    protected:
        TagVecDictionaryNode valvec[CYCLE_CHUNK_SIZE]; // fixed size 4096 entries...
        INT32 nextEntry; // Position of the next value.
        INT32 baseCycle; // Cycle where the data starts.

    protected:
        static Dict2064* dict; // Pointer to the dictionary.
        static StrTable* strtbl; // Pointer to the string table.
};

/**
 * Adds a new tag in the vector.
 *
 * @return true if everything is ok.
 */
bool
TagVecDictionary::addTagValue(INT32 cycle, UINT64 value)
{
    //printf ("TagVecDictionary::addTagValue(cycle=%d,value=%llu\n",cycle,value);fflush(stdout);

    INT32 offset = cycle - baseCycle;
    Q_ASSERT ((offset>=0)&&(offset<CYCLE_CHUNK_SIZE));
    Q_ASSERT (nextEntry<CYCLE_CHUNK_SIZE);
    INT32 dkey = dict->getKeyFor(value,cycle);

    // check if is the same value than previous set
    if ((nextEntry>0) && ((UINT32)dkey==valvec[nextEntry-1].key) )
    {
        return true;
    }

    // Inserts the value.
    valvec[nextEntry].key = (UINT32)dkey;
    valvec[nextEntry].cycleOffset = (UINT32)offset;
    ++nextEntry;
    return true;
}

/**
 * Adds a new tag in the vector.
 *
 * @return true if everything is ok.
 */
bool
TagVecDictionary::addTagValue(INT32 cycle, QString  value)
{
    //printf ("TagVecDictionary::addTagValue(cycle=%d,value=%s\n",cycle,value.latin1());fflush(stdout);

    INT32 offset = cycle-baseCycle;
    Q_ASSERT ((offset>=0)&&(offset<CYCLE_CHUNK_SIZE));
    Q_ASSERT(nextEntry<CYCLE_CHUNK_SIZE);

    UINT64 nvalue = (UINT64)(strtbl->addString(value));
    INT32 dkey = dict->getKeyFor(nvalue,cycle);

    // check if is the same value than previous set
    if ((nextEntry>0) && ((UINT32)dkey==valvec[nextEntry-1].key) )
    {
        return true;
    }

    valvec[nextEntry].key = (UINT32)dkey;
    valvec[nextEntry].cycleOffset = (UINT32)offset;
    ++nextEntry;
    return true;
}

/**
 * Adds a new tag in the vector.
 *
 * @return true if everything is ok.
 */
bool
TagVecDictionary::addTagValue(INT32 cycle, SOVList* value)
{
    // TODO.
    return false;
}

/**
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecDictionary::getType()
{
    return TVEType_DICTIONARY;
}

/**
 * Gets the value of this tag in the cycle cycle.
 *
 * @return true if the value has been found.
 */
bool
TagVecDictionary::getTagValue(INT32 cycle, UINT64*  value, UINT32* atcycle)
{
    //if (!fwd) return getTagValueNoFw(cycle,value);
    INT32 i=0;
	UINT32 ccycle = valvec[i].cycleOffset+baseCycle;
    bool somethingfnd = ccycle <= (UINT32)cycle ;
    while ( (i<nextEntry) && ((valvec[i].cycleOffset+baseCycle) <= (UINT32)cycle) )
    {
		ccycle = valvec[i].cycleOffset+baseCycle;
        *value = dict->getValueByCycle(valvec[i].key,ccycle);
        ++i;
    }
	if (atcycle!=NULL)
    {
        *atcycle = ccycle;
    }
    return somethingfnd;
}

/**
 * Gets the value of this tag in the cycle cycle.
 *
 * @return true if the value has been found.
 */
bool
TagVecDictionary::getTagValue(INT32 cycle, SOVList** value, UINT32* atcycle)
{
    /// @todo implement this.
    return false;
}

#endif
