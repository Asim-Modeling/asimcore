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
 * @file dralStringMapping.h
 * @author Guillem Sole
 * @brief A mapping of strings to integers.
 */

#ifndef _DRALSTRINGMAPPING_H
#define _DRALSTRINGMAPPING_H

// ASIM includes.
#include "asim/dral_syntax.h"

// General includes.
#include <map>
#include <list>
#include <string>

using namespace std;

/**
  * @brief Class used to map strings to indexes
  *
  * This class is used to send integer instead of strings in
  * the different DRAL LISTENER callbacks. Sending integers
  * instead of strings, allows a more compacted trace and also
  * a faster interface of the listener.
  * The class basically offers just one function, that is the
  * mapping between string and a value. When a string is requested
  * the mapping is returned and if is not found in the mapping, a
  * new index is stored and returned. As the user can specify the
  * maximum number of strings to remember, this mapping might kill
  * some entries. The policy implemented by now is LRU.
  */
class DRAL_STRING_MAPPING_CLASS
{
  public:

    /**
      * @brief Creator of the class.
      * @param size The maximum number of strings that can remember.
      */
    DRAL_STRING_MAPPING_CLASS(UINT32 size);

    /**
      * @brief Destructor of the class.
      */
    ~DRAL_STRING_MAPPING_CLASS();

    /**
      * @brief This function performs the mapping between the string
      *        and the index.
      * @param str The string to map to an integer.
      * @param strlen The length of the string (including 0).
      * @param index Pointer where store the mapping.
      * @return true if str isn't in the mapping, so a callback to add this
      *         string in the mapping is needed.
      */
    bool getMapping(const char * str, UINT16 strlen, UINT32 * index);

  private:
    /**
     * Structs used to implement the LRU policy using stl lists.
     */
    typedef list<string> LRUList;
    typedef list<string>::iterator LRUListIterator;

    /**
     * Struct associated to each of the entries of the mapping. The LRU
     * iterator is used to have direct access to the elements of the list,
     * so when a string is requested, the entry can be moved directly
     * to the end of the LRU without a linear search.
     */
    struct StringMappingEntry
    {
        LRUListIterator it; ///< Iterator to the LRU entry.
        UINT32 index;       ///< Mapping between of the string.
    };

    /**
     * Structs to implement the mapping using a stl hash map.
     */
    typedef map<string, StringMappingEntry> StringMapping;
    typedef map<string, StringMappingEntry>::iterator StringMappingIterator;

    /**
     * For debug purpose.
     */
    void dump();

  private:
    UINT32 max_strs;       ///< Maximum number of strings.
    LRUList lru;           ///< The LRU list.
    StringMapping mapping; ///< The map between strings and integers.
    UINT32 commands;       ///< Internal statistics: number of new strings needed.
    UINT32 conflicts;      ///< Internal statistics: number of conflicts in the map.
} ;

typedef DRAL_STRING_MAPPING_CLASS * DRAL_STRING_MAPPING; /**<Pointer type to
                                                          * a DRAL_STRING_MAPPING_CLASS
                                                          */

#endif /* DRAL_STRING_MAPPING_CLASS */
