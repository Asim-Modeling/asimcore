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

#ifndef _DRALDB_HASH6431_H
#define _DRALDB_HASH6431_H

#include <qnamespace.h>

#include "asim/draldb_syntax.h"

class Hash6431;

class Hash6431Node
{
    public:
        /**
         * Creates a void hash node.
         *
         * @return the new object.
         */
        Hash6431Node()
        {
            next=NULL;
            key=0;
            value=0;
            used=0;
        }

        /**
         * Nothing done.
         *
         * @return destroys the object.
         */
        ~Hash6431Node()
        {
        }

    protected:
        Hash6431Node* next; // Pointer to the next collision node.
        UINT64 key; // Key of the entry.
        INT32  value : 31; // Value of the entry.
        UINT32  used  : 1; // If the node is occupied or not.

    friend class Hash6431;
};

/**
  * @brief
  * Integer hash table that maps 64 bit integers to 32 bits.
  *
  * @description
  * The hash is implemented using a collision list when two
  * different entries map to the same bucket. The hashing funciton
  * is just a mod.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class Hash6431
{
    public:
        Hash6431(INT32 buckets);
        ~Hash6431();

        inline INT32 find(UINT64 key);
        inline void  insert(UINT64 key, INT32 value);

        void clear();
    private:
        Hash6431Node* bvector; // Pointer to the hash nodes.
        INT32         buckets; // Number of buckets of the hash.
};

/**
 * This function hashes the key and searches through the collision
 * list of the hashed value until it finds the node that has the
 * value for the key. If the key is not found a -1 is returned.
 *
 * @return the value mapped to the key.
 */
INT32
Hash6431::find(UINT64 key)
{
    INT32 lastValue=-1;
    UINT64 bk = key % buckets;
    Hash6431Node* current = &(bvector[bk]);
    if (! (current->used) )
    {
        return lastValue;
    }

    bool fnd = false;
    // Until the key is found or the end of the list is reached.
    while (!fnd && (current !=NULL))
    {
        fnd = (current->used) && (current->key == key);
        lastValue = current->value;
        current = current->next;
    }
    if (!fnd)
    {
        lastValue = -1;
    }
    return lastValue;
}

/**
 * Inserts a value associated to the key in the hash. If the bucket
 * of the hashed key is already occupied the entry is stored at the
 * end of the list.
 *
 * @return void.
 */
void
Hash6431::insert(UINT64 key, INT32 value)
{
    UINT64 bk = key % buckets;
    Hash6431Node* current = &(bvector[bk]);
    Hash6431Node* prev = current;
    // If the bucket is already used.
    if (current->used)
    {
        // Goes to the end of the collision list.
        while (current!=NULL)
        {
            prev = current;
            current = current->next;
        }
        // Allocate a new node.
        current = new Hash6431Node;
        prev->next = current;
    }
    current->used = 1;
    current->key = key;
    current->value = value;
}

#endif
