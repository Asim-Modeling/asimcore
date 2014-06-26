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

#ifndef _DRALDB_STRTABLE_H
#define _DRALDB_STRTABLE_H

// standard C
#include <stdio.h>
#include <stdlib.h>

// QT library
#include <qstring.h>
#include <q3dict.h>
#include <q3intdict.h>

#include "asim/draldb_syntax.h"
#include "asim/AMemObj.h"
#include "asim/DralDBDefinitions.h"
#include "asim/StatObj.h"

/**
  * @brief
  * This class holds a mapping between strings and integers.
  *
  * @description
  * We keep a huge amount of symbols (string) and this table avoid
  * repeated strings to spend memory space. This class has just one
  * object instanstiated, and the other classes have access to it
  * calling the getInstance function.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */

class StrTable : public AMemObj, public StatObj
{
    public:
        // ---- AMemObj Interface methods
        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------

    public:
        static StrTable* getInstance ();
        static StrTable* getInstance (INT32 sz);
        static void destroy();

    public:
        inline INT32 addString(QString);
        inline QString getString(INT32);
        inline bool hasString(QString);

        void reset();
        void resize(INT32 newsize);

    protected:
        StrTable(INT32 sz);
        virtual ~StrTable();
        void init(INT32 sz);

        inline INT32 lookForStr(QString);

    private:
        Q3Dict<INT32>*      strhash; // Hash from string to int.
        Q3IntDict<QString>* idxhash; // Hash from int to string.

        INT32 nextIdx;
        INT32 primeSize;

    private:
        static StrTable* _myInstance;
};

/**
 * Returns the string mapped to the integer idx. If no string is
 * mapped to this integer a null is returned.
 *
 * @return the string.
 */
QString
StrTable::getString(INT32 idx)
{
    QString *str = idxhash->find((long)idx);
    if (str==NULL)
    {
        return (QString::null);
    }
    else
    {
        return (*str);
    }
}

/**
 * Adds a string to the table and returns the index mapped to the
 * string.
 *
 * @return the indexed mapped to the string.
 */
INT32 
StrTable::addString(QString str)
{
    // look for the string to avoid redundant entries
    INT32* idx_ptr = strhash->find(str);

    if (idx_ptr == NULL) // not found
    {
        idx_ptr = new INT32;
        Q_ASSERT(idx_ptr!=NULL);
        *idx_ptr = nextIdx;
        strhash->insert(str,idx_ptr);
        QString *tmpstr = new QString(str);
        Q_ASSERT(tmpstr!=NULL);
        idxhash->insert((long)nextIdx,tmpstr);
        nextIdx++;
    }
    return (*idx_ptr);
}

/**
 * Looks for the integer mapped to the string str.
 *
 * @return the index mapped to the string.
 */
INT32
StrTable::lookForStr(QString str)
{
   INT32* idx_ptr = strhash->find(str);
   if (idx_ptr == NULL)
   {
       return (-1);
   }
   else
   {
       return (*idx_ptr);
   }
}

/**
 * Looks if the string is already in the table.
 *
 * @return true if the index is in the table. False otherwise.
 */
bool
StrTable::hasString(QString str)
{
    return (lookForStr(str)>=0);
}

#endif
