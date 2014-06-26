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

#ifndef _DRALDB_TAGVEC_H
#define _DRALDB_TAGVEC_H

#include <stdio.h>

#include <qstring.h>

#include "asim/draldb_syntax.h"
#include "asim/DRALTag.h"
#include "asim/ZipObject.h"

/**
  * @brief
  * Abstract class to hide encoding formats.
  *
  * @description
  * This class is an interface of different vector implementations.
  * Each implementation is used to know the value of a tag in a
  * precise moment. Each version differs from each other by the way
  * this matching is done : only matches if the tag was set in the
  * same cycle that is requested, if was set before, ...
  *
  * @version 0.1
  * @date started at 2003-05-27
  * @author Federico Ardanaz
  */

typedef enum
{
    TVEType_ITEMIDX,                // for moveitems, enter/exitnodes
    TVEType_DENSE_ITEMIDX,
    TVEType_DENSE_SHORT_ITEMIDX,
    TVEType_DICTIONARY,             // for node tags
    TVEType_DENSE_DICTIONARY,
    TVEType_DICTIONARY_NF,          // for cycle tags
    TVEType_DENSE_DICTIONARY_NF
} TagVecEncodingType;

class TagVec : public ZipObject
{
    public:
        inline TagVec(){ we=true; pendingCnt=0; }

        virtual bool getTagValue(INT32 cycle, UINT64*  value, UINT32* atcycle) = 0;
        virtual bool getTagValue(INT32 cycle, SOVList** value, UINT32* atcycle) = 0;

        virtual bool addTagValue(INT32 cycle, UINT64   value) = 0;
        virtual bool addTagValue(INT32 cycle, QString  value) = 0;
        virtual bool addTagValue(INT32 cycle, SOVList* value) = 0;

        virtual void dumpCycleVector() = 0;
        virtual TagVecEncodingType getType() = 0;

        inline virtual bool isWriteEnabled();
        inline virtual void setWriteEnabled(bool);
        inline virtual void incPendingCnt();
        inline virtual void decPendingCnt();

   protected:
        bool  we;
        INT32 pendingCnt;
};

/**
 * Returns the value of the we flag.
 *
 * @return we flag.
 */
bool
TagVec::isWriteEnabled()
{
    return we;
}

/**
 * Sets the we flag.
 *
 * @return void.
 */
void
TagVec::setWriteEnabled(bool value)
{
    we = value;
}

/**
 * Increments pending counter.
 *
 * @return void.
 */
void
TagVec::incPendingCnt()
{
    ++pendingCnt;
    we=true;
    //printf("TagVec::incPendingCnt called => %d\n",pendingCnt);
}

/**
 * Decrements the pending counter.
 *
 * @return void.
 */
void
TagVec::decPendingCnt()
{
    --pendingCnt;
    //printf("TagVec::decPendingCnt called => %d\n",pendingCnt);
    if (pendingCnt==0)
    {
        we = false;
    }
}

#endif
