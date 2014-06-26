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

#ifndef _DRALDBDEFINITIONS_H_
#define _DRALDBDEFINITIONS_H_

#define DRALDB_MAJOR_VERSION 0
#define DRALDB_MINOR_VERSION 7
#define DRALDB_STR_MAJOR_VERSION "0"
#define DRALDB_STR_MINOR_VERSION "7"

// -------------------------------------------------
// TagDescVector definitions
// -------------------------------------------------
#define TAGDESCVEC_SIZE 4096

// -------------------------------------------------
// StrTable definitions
// -------------------------------------------------
#define STRTBL_DEFAULT_SIZE     10000

// -------------------------------------------------
// LiveDB definitions
// -------------------------------------------------
#define LIVEDB_DEFAULT_MAXITEMCNT 100000

// -------------------------------------------------
// Cycle bitfield-related definitions
// -------------------------------------------------
#define CYCLE_CHUNK_SIZE 4096
#define CYCLE_OFFSET_BITS 12
#define CYCLE_OFFSET_MASK 0x00000FFFU

// -------------------------------------------------
// DBGraph definitions
// -------------------------------------------------
/**
  * @def DEFAULT_DGN_SIZE
  * @brief This is default size used for hashed list of Dral Nodes.
  * Bear in mind that this MUST be a prime number.
  */
#define DEFAULT_DBGN_SIZE 2111

/**
  * @def DEFAULT_DGE_SIZE
  * @brief This is default size used for hashed list of Dral Edges.
  * Bear in mind that this MUST be a prime number.
  */
#define DEFAULT_DBGE_SIZE 99991


#define CACHED_ITEMIDS 10000

// -------------------------------------------------
// Global definitions
// -------------------------------------------------

/** @def used as a "canonical" item id string */
#define ITEMID_STR_TAG "ITEMID"

/** @def used as a "canonical" item id IN string */
#define ITEMIDIN_STR_TAG "_ITEMID_IN"

/** @def used as a "canonical" item id IN string */
#define ITEMIDXIN_STR_TAG "_ITEMIDX_IN"

/** @def used as special internal use pointer to ItemTagHeap */
#define ITEMIDX_STR_TAG "_DRALDB_ITEMIDX_ON_ITEMTAGHEAP"

#define AGEPURGE_STR_TAG "_AGEPURGED"
#define EOFPURGE_STR_TAG "_EOFPURGED"

#endif


