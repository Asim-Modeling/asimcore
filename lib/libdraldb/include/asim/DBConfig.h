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

#ifndef _DRALDB_DBCONFIG_H
#define _DRALDB_DBCONFIG_H

#include "asim/draldb_syntax.h"

#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

/**
  * @brief
  * This class holds the DB configuration values.
  *
  * @description
  * Just defines few methods to get and set the different
  * configuration values of this data base.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DBConfig
{
    public:
        static DBConfig* getInstance ();
        static void destroy();

    public:
        inline bool getAutoPurge() ;
        inline bool getIncrementalPurge() ;
        inline bool getMaxIFIEnabled() ;
        inline bool getTagBackPropagate() ;
        inline bool getGUIEnabled() ;
        inline bool getCompressMutable() ;
        inline INT32  getItemMaxAge() ;
        inline INT32  getMaxIFI() ;

        inline void setAutoPurge(bool value) ;
        inline void setIncrementalPurge(bool value) ;
        inline void setMaxIFIEnabled(bool value) ;
        inline void setTagBackPropagate(bool value) ;
        inline void setGUIEnabled(bool value) ;
        inline void setCompressMutable(bool value) ;
        inline void setItemMaxAge(INT32  value) ;
        inline void setMaxIFI(INT32  value) ;

		void reset();
		
    protected:
        DBConfig();
        ~DBConfig();
		
    private:
        bool guiEnabled; // True if the GUI is enabled.
        bool autoPurge; // Auto purge the non deleted items when the end of simulation is reached.
        bool incrementalPurge;
        bool maxIFIEnabled;
        bool tagBackPropagate; // Propagate tag values back.
        bool compressMutable; // Compress mutable tags in the ItemTagHeap struct.
        INT32  itemMaxAge; // Maximum cycles an item can be alive.
        INT32  maxIFI;

    private:
       static DBConfig* _myInstance; // Instance of the class.
};

/**
 * Returns the autopurge value.
 *
 * @return autopurge.
 */
bool
DBConfig::getAutoPurge()
{
    return autoPurge;
}

/**
 * Returns the incrementalPurge value.
 *
 * @return incrementalPurge.
 */
bool
DBConfig::getIncrementalPurge()
{
    return incrementalPurge;
}

/**
 * Returns the maxIFIEnabled value.
 *
 * @return maxIFIEnabled.
 */
bool
DBConfig::getMaxIFIEnabled()
{ 
    return maxIFIEnabled;
}

/**
 * Returns the tagBackPropagate value.
 *
 * @return tagBackPropagate.
 */
bool
DBConfig::getTagBackPropagate()
{
    return tagBackPropagate;
}

/**
 * Returns the guiEnabled value.
 *
 * @return guiEnabled.
 */
bool
DBConfig::getGUIEnabled()
{
    return guiEnabled;
}

/**
 * Returns the compressMutable value.
 *
 * @return compressMutable.
 */
bool
DBConfig::getCompressMutable()
{
    return compressMutable;
}

/**
 * Returns the itemMaxAge value.
 *
 * @return itemMaxAge.
 */
INT32
DBConfig::getItemMaxAge()
{
    return itemMaxAge;
}

/**
 * Sets the maxIFI value.
 *
 * @return maxIFI.
 */
INT32
DBConfig::getMaxIFI()
{
    return maxIFI;
}

/**
 * Sets the autoPurge value.
 *
 * @return void.
 */
void
DBConfig::setAutoPurge(bool value)
{
    autoPurge = value;
}

/**
 * Sets the incrementalPurge value.
 *
 * @return void.
 */
void
DBConfig::setIncrementalPurge(bool value)
{
    incrementalPurge = value;
}

/**
 * Sets the maxIFIEnabled value.
 *
 * @return void.
 */
void
DBConfig::setMaxIFIEnabled(bool value)
{
    maxIFIEnabled = value;
}

/**
 * Sets the tagBackPropagate value.
 *
 * @return void.
 */
void
DBConfig::setTagBackPropagate(bool value)
{
    tagBackPropagate = value;
}

/**
 * Sets the guiEnabled value.
 *
 * @return void.
 */
void
DBConfig::setGUIEnabled(bool value)
{
    guiEnabled = value;
}

/**
 * Sets the compressMutable value.
 *
 * @return void.
 */
void
DBConfig::setCompressMutable(bool value)
{
    compressMutable = value;
}

/**
 * Sets the itemMaxAge value.
 *
 * @return void.
 */
void
DBConfig::setItemMaxAge(INT32  value)
{
    itemMaxAge = value;
}

/**
 * Sets the maxIFI value.
 *
 * @return void.
 */
void
DBConfig::setMaxIFI(INT32  value)
{
    maxIFI = value;
}

#endif
