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

#ifndef _AMEMOBJ_H
#define _AMEMOBJ_H

// QT library
#include <qstring.h>

#include "asim/draldb_syntax.h"

/**
  * @brief
  * This interface defines a few methods used to gather information
  * about memory usage.
  *
  * @description
  * Any (non size critical) dynamically allocated object in an AGT
  * application (2Dreams,etc) must inherit from AMemObj. This is a
  * simple mechanism to be able to know (in detail) the amount of
  * dynamically allocated memory used at any moment.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class AMemObj
{
    public:

        /**
         * A simple object would just return sizeof(this)a composite
         * one is responsable of adding the size of its subcomponents.
         *
         * @return the size of the object.
         */
        virtual INT64 getObjSize() const = 0;

        /**
         * To know if all internal dynamically allocated objects were
         * sucessfully allocated:
         *
         * @return success allocate.
         */
        //virtual bool sucessfullAllocated() const = 0;

        /**
         * Textual (detailed) description of the memory usage this
         * make sense mainly for big composite objects.
         *
         * @return the description .
         */
        virtual QString getUsageDescription() const;
};

#endif
