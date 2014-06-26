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

#ifndef _DRALDB_TAGVECDENSEDICTIONARYNF_H
#define _DRALDB_TAGVECDENSEDICTIONARYNF_H

#include "asim/DralDBDefinitions.h"
#include "asim/TagVec.h"
#include "asim/StrTable.h"
#include "asim/Dict2064.h"
#include "asim/TagVecDictionaryNF.h"

/**
  * @brief
  * This class is a dense tag vector that uses a dictionary.
  *
  * @description
  * This implementation just alloc the necessary space to store
  * all the values. When a value is requested for one cycle, the
  * function only returns match if the set of the value is in the
  * same cycle.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class TagVecDenseDictionaryNF : public TagVec
{
    public:
        TagVecDenseDictionaryNF(TagVecDictionaryNF* original);
        virtual ~TagVecDenseDictionaryNF();

        inline bool getTagValue(INT32 cycle, UINT64*  value, UINT32* atcycle);
        inline bool getTagValue(INT32 cycle, SOVList** value, UINT32* atcycle);

        inline TagVecEncodingType getType();

        inline bool addTagValue(INT32 cycle, UINT64   value);
        inline bool addTagValue(INT32 cycle, QString  value);
        inline bool addTagValue(INT32 cycle, SOVList* value);

        inline ZipObject* compressYourSelf(INT32 cycle, bool last=false);

        void dumpCycleVector();

    private:
        TagVecDictionaryNode* valvec; // Pointer to the array of data.
        INT32 nextEntry; // Position where the next entry will be added.
        INT32 baseCycle; // Base cycle of the vector.

    private:
        static Dict2064* dict; // Pointer to the dictionary of values.
        static StrTable* strtbl; // Pointer to the string table.
};

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVecDenseDictionaryNF::addTagValue(INT32 cycle, UINT64   value)
{
    // a posteriori additions not supported
    return false;
}

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVecDenseDictionaryNF::addTagValue(INT32 cycle, QString  value)
{
    // a posteriori additions not supported
    return false;
}

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVecDenseDictionaryNF::addTagValue(INT32 cycle, SOVList* value)
{
    // a posteriori additions not supported
    return false;
}

/**
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecDenseDictionaryNF::getType()
{
    return TVEType_DENSE_DICTIONARY_NF;
}

/**
 * Gets the value of this tag in the cycle cycle.
 *
 * @return true if the value has been found.
 */
bool
TagVecDenseDictionaryNF::getTagValue(INT32 cycle, UINT64* value, UINT32* atcycle)
{
    INT32 i=0;
    bool fnd = false ;
	UINT32 ccycle = 0;
    while ( (i<nextEntry) && !fnd && ((valvec[i].cycleOffset+baseCycle)<=(UINT32)cycle) )
    {
		ccycle = valvec[i].cycleOffset+baseCycle;
        fnd = ccycle == (UINT32)cycle;
        *value = dict->getValueByCycle(valvec[i].key,ccycle);
        ++i;
    }
	if (fnd && (atcycle!=NULL) ) { *atcycle = ccycle; }
    return fnd;
}

/**
 * Gets the value of this tag in the cycle cycle.
 *
 * @return true if the value has been found.
 */
bool
TagVecDenseDictionaryNF::getTagValue(INT32 cycle, SOVList** value, UINT32* atcycle)
{
    /// @todo implement this.
    return false;
}

/**
  * Object already compressed.
  *
  * @return itself.
  */
ZipObject*
TagVecDenseDictionaryNF::compressYourSelf(INT32 cycle, bool last)
{
    return this;
}

#endif
