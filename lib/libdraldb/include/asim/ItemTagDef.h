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

#ifndef _DRALDB_TAGITEMDEF_H_
#define _DRALDB_TAGITEMDEF_H_

#include "asim/draldb_syntax.h"
#include "asim/AEVector.h"

#define TAGS_PER_CHUNK 4

/** @typdef ItemTagHeapNode
  * @brief
  * Contains all the information necessary to know what tag is
  * holding, which value has, which type of tag is and when the
  * value was set. 64 bits.
  */
typedef struct
{
    UINT32 tagId     : 12; // Id of the tag.
    UINT32 dkey      : 20; // Dictionary key value of the tag.
    UINT32 isMutable :  1; // Is a mutable tag.
    UINT32 isSOV     :  1; // Is a set of values tag.
    UINT32 cycle     : 22; // Cycle where the tag was set.
    UINT32 reserved  :  8; // Reserved bits that are used in the ItemTagHeapChunk struct.
} TagHeapNode;

/**
  * @brief
  * Chunk of four tags of an item.
  *
  * @description
  * Uses the reserved bits of each ItemTagHeapNode to know the
  * pointer to the next chunk, and which is the first free slot
  * of the chunk. For this pourpose uses 28 bits for the next
  * chunk and 3 bits for the first free slot. This last field
  * uses the 3 high bits of the first tag of the chunk to store
  * its information and the next chunk pointer uses all the
  * reserved bits of the other 3 tags and the 4 lower bits of the
  * first tag. By now, 1 bit remains unused.
  *
  * @version 0.1
  * @date started at 2004-05-25
  * @author Guillem Sole
  */
class TagHeapChunk
{
    public :
        /*
         * Creator of the class.
         *
         * @return the new object.
         */
        TagHeapChunk()
        {
        }

        /*
         * Returns the first free tag slot of the chunk.
         *
         * @return slot position.
         */
        UINT32
        getFirstFreeSlot()
        {
            return (content[0].reserved >> 5);
        }

        /*
         * Sets the first free tag slot of the chunk. Must be lower
         * or equal than TAGS_PER_CHUNK.
         *
         * @return void.
         */
        void
        setFirstFreeSlot(UINT32 slot)
        {
            Q_ASSERT(slot <= TAGS_PER_CHUNK);
            content[0].reserved = static_cast<unsigned char>((content[0].reserved & 0x1F) | (slot << 5));
        }

        /*
         * Allocates a slot incrementing the occupancy.
         *
         * @return void.
         */
        UINT32
        allocateSlot()
        {
            UINT32 temp;

            temp = content[0].reserved >> 5;
            temp++;
            Q_ASSERT(temp <= TAGS_PER_CHUNK);
            content[0].reserved = static_cast<unsigned char>((content[0].reserved & 0x1F) | (temp << 5));
            return (temp - 1);
        }

        /*
         * Returns the pointer to the next chunk.
         *
         * @return the vector position.
         */
        UINT32
        getNextChunk()
        {
            return (((content[0].reserved & 0x0F) << 24)
                    | (content[1].reserved << 16)
                    | (content[2].reserved << 8)
                    | content[3].reserved);
        }

        /*
         * Sets the first free tag slot of the chunk. Must be lower
         * or equal than TAGS_PER_CHUNK.
         *
         * @return void.
         */
        void
        setNextChunk(UINT32 slot)
        {
            content[0].reserved = static_cast<unsigned char>((content[0].reserved & 0xE0) | ((slot >> 24) & 0x0F));
            content[1].reserved = static_cast<unsigned char>((slot >> 16) & 0xFF);
            content[2].reserved = static_cast<unsigned char>((slot >> 8) & 0xFF);
            content[3].reserved = static_cast<unsigned char>(slot & 0xFF);
        }

        TagHeapNode content[TAGS_PER_CHUNK]; // Four tags per chunk.
} ;

/** @typdef ItemTagHeapVector
  * @brief
  * Dynamic array that holds all the tags of the items of the heap.
  * Up to 268*10^6 chunks. Needs 28 bits to index within the vector.
  */
typedef AEVector<TagHeapChunk, 32768, 8192> TagHeapVector;

/** @typdef ItemHeapNode
  * @brief
  * Stores the information about a created item. Just needs to
  * save its id and the pointer to the first chunk of tags (zero
  * means no valid entry).
  */
typedef struct
{
    INT32 itemId;       // Id of the item entry.
    UINT32 first_chunk; // Pointer to the first chunk of tags of this item.
} ItemHeapNode;

/** @typdef ItemHeapVector
  * @brief
  * Dynamic array that holds all the items of the heap. Up to
  * 268*10^6 items. Needs 28 bits to index within the vector.
  */
typedef AEVector<ItemHeapNode, 32768, 8192> ItemHeapVector;

#define MAX_ITEM_ENTRIES 32768 * 8192

#endif
