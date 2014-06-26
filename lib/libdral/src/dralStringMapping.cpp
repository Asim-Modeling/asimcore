/**************************************************************************
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

/**
 * @file dralStringMapping.cpp
 * @author Guillem Sole
 * @brief A mapping of strings.
 */

#include <stdlib.h>
#include <iostream>
#include <assert.h>

#include "asim/dralStringMapping.h"

DRAL_STRING_MAPPING_CLASS::DRAL_STRING_MAPPING_CLASS(UINT32 size)
{
    assert(size > 0);
    max_strs = size;
    commands = 0;
    conflicts = 0;
}

DRAL_STRING_MAPPING_CLASS::~DRAL_STRING_MAPPING_CLASS()
{
}

bool
DRAL_STRING_MAPPING_CLASS::getMapping(const char * str, UINT16 strlen, UINT32 * index)
{
    StringMappingIterator it_map;

    // Internal statistic update.
    commands++;

    // Tries to find the string in the mapping.
    it_map = mapping.find(str);

    if(it_map == mapping.end())
    {
        UINT32 new_index;         ///< Index that will be used.
        StringMappingEntry entry; ///< New entry of the mapping.

        // If not found, checks if we've space in the mapping.
        if(mapping.size() == max_strs)
        {
            LRUListIterator it_lru;
            StringMappingIterator it_map2;

            // Internal statistic update.
            conflicts++;

            // Gets the entry to remove.
            it_lru = lru.begin();
            it_map2 = mapping.find(* it_lru);

            // Recycles the index.
            new_index = it_map2->second.index;

            // Removes it from the mapping.
            mapping.erase(it_map2);
            lru.pop_front();
        }
        else
        {
            // The index set is equal to the size of the mapping.
            new_index = mapping.size();
        }

        // Adds the new entry in the lru and the mapping.
        lru.push_back(str);
        entry.index = new_index;
        entry.it = --lru.end();

        mapping[str] = entry;
        * index = new_index;

        return true;
    }
    else
    {
        // If found, just sets the index and returns that no new callback is needed and also
        // updates the lru state.
        lru.erase(it_map->second.it);
        lru.push_back(it_map->first);
        it_map->second.it = --lru.end();
        * index = it_map->second.index;
        return false;
    }
}

void
DRAL_STRING_MAPPING_CLASS::dump()
{
    StringMappingIterator it;
    const char ** temp;

    temp = (const char **) malloc(max_strs * sizeof(const char *));

    cout << "Dumping string map:" << endl;

    for(UINT32 i = 0; i < max_strs; i++)
    {
        temp[i] = NULL;
    }

    for(it = mapping.begin(); it != mapping.end(); it++)
    {
        temp[it->second.index] = it->first.c_str();
    }

    for(UINT32 i = 0; i < max_strs; i++)
    {
        cout << "\tEntry " << i << " is: " << temp[i] << endl;
    }
    cout << endl;

    free(temp);
}
