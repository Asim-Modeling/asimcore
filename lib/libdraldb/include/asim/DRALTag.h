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

#ifndef _DRALTAG_H
#define _DRALTAG_H

#include "asim/draldb_syntax.h"
#include "asim/fvaluevector.h"

#include "qglobal.h"
#include "qstring.h"

/**
  * @brief
  * This is a small enumeration for the different types of values
  * a tag can hold on.
  *
  * Put long explanation here
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
typedef enum
{
    TagUnknownType = 0,
    TagStringValue,
    TagIntegerValue,        ///< 64 bits UINT64
    TagFPValue,             ///< 64 bits IEEE double
    TagSetOfValues          ///< list of values (INT64)
} TagValueType;

/** @typdef SOVList
  * @brief
  * Set of values is a vector of unsigned integers.
  */
typedef FValueVector<UINT64> SOVList;

/**
  * @brief
  * This class represents the capacity of a node with a defined
  * layout.
  * WARNING!!! WARNING!!! WARNING!!!
  * Be careful when dealing with this class: when the object is
  * destroyed it automatically deletes the vector. Don't have
  * various elements pointing to the same allocated array of
  * integers!!!
  */
class NodeSlot
{

    public :
        /**
         * Creator of this class. Sets default values.
         *
         * @return new object.
         */
        NodeSlot()
        {
            dimVec = NULL;
            specDimensions = 0;
        }

        /**
         * Copy creator. Makes a copy of a previous NodeSlot.
         *
         * @return new object.
         */
        NodeSlot(const NodeSlot& slot)
        {
            copyVector(slot.specDimensions, slot.dimVec);
        }

        /**
         * Creator with dimension parameter
         *
         * @return new object.
         */
        NodeSlot(UINT16 numDim)
        {
            specDimensions = numDim;
            dimVec = new UINT32[numDim];
        }

        /**
         * Creator with vector parameter
         *
         * @return new object.
         */
        NodeSlot(UINT16 numDim, UINT32 * dim)
        {
            copyVector(numDim, dim);
        }

        /**
         * Destructor of this class. Frees the vector if necessary.
         *
         * @return destroys the object.
         */
        ~NodeSlot()
        {
            if(dimVec != NULL)
            {
                delete [] dimVec;
            }
        }

        /**
         * Assignation operator of this class.
         *
         * @return void.
         */
        void
		operator=(const NodeSlot& orig)
        {
            if(dimVec != NULL)
            {
                delete [] dimVec;
            }
            copyVector(orig.specDimensions, orig.dimVec);
        }

        /**
         * Comparison of two node slots.
         *
         * @return if the two node slots are equal.
         */
        bool
		operator==(const NodeSlot& cmp)
        {
            if(specDimensions != cmp.specDimensions)
            {
                return false;
            }
            else
            {
                for(UINT16 i = 0; i < specDimensions; i++)
                {
                    if(dimVec[i] != cmp.dimVec[i])
                    {
                        return false;
                    }
                }
                return true;
            }
        }

        /**
         * Clears the content of the slot.
         *
         * @return void.
         */
		void
		clearSlot()
		{
			for(UINT16 i = 0; i < specDimensions; i++)
			{
				dimVec[i] = 0;
			}
		}

		/**
         * Dumps the content of the node slot.
         *
         * @return a string with the dump.
         */
        QString
		dump()
        {
            QString ret = "";

            ret = "Dim " + QString::number(specDimensions);
            if(specDimensions > 0)
            {
                ret += ": ";
                for(UINT16 i = 0; i < specDimensions - 1; i++)
                {
                    ret += QString::number(dimVec[i]) + ", ";
                }
                ret += QString::number(dimVec[specDimensions - 1]);
            }
            return ret;
        }

    private :
        /**
         * Copies a integer vector to the NodeSlot.
         *
         * @return void.
         */
        void
		copyVector(UINT16 numDim, UINT32 * dim)
        {
            specDimensions = numDim;
            if(specDimensions != 0)
            {
                dimVec = new UINT32[specDimensions];
                for(UINT32 i = 0; i < specDimensions; i++)
                {
                    dimVec[i] = dim[i];
                }
            }
            else dimVec = NULL;
        }

    public :
        UINT16 specDimensions; ///< Number dimensions of the slot.
        UINT32 * dimVec;       ///< Capacity of each dimension.
} ;

#endif
