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


#ifndef __PLRU_H__
#define __PLRU_H__

#include "asim/syntax.h"
#include "asim/mesg.h"
#include "asim/utils.h"
#include "asim/ioformat.h"
#include "asim/trace.h"
#include "asim/cache_manager.h"

template<int Ways>
class PLRU_MaskInner{
  /* This class is a placeholder: it should only be used for one of the 
     template specification'ed instances below */
};




template<>
class PLRU_MaskInner<1> {
public:
  static const UINT64  mask[1];     
  static const UINT64 compare[1];  
};


template<>
class PLRU_MaskInner<2> {
public:
  static const UINT64  mask[2];     
  static const UINT64 compare[2];  
};


template<>
class PLRU_MaskInner<3> {
public:
  static const UINT64 mask[3]   ;
  static const UINT64 compare[3] ;
};



template<>
class PLRU_MaskInner<4> {
public:
  static const UINT64 mask[4];    
  static const UINT64 compare[4]; 
};




template<>
class PLRU_MaskInner<6> {
public:
  static const UINT64 mask[6];   
  static const UINT64 compare[6];
};


template<>
class PLRU_MaskInner<8> {
public:
  static const UINT64 mask[8];    
  static const UINT64 compare[8];
};


template<>
class PLRU_MaskInner<12> {
public:
  static const UINT64 mask[12];
  static const UINT64 compare[12];
};


template<>
class PLRU_MaskInner<16> {
public:
  static const UINT64 mask[16]; 
  static const UINT64 compare[16];
};

template<>
class PLRU_MaskInner<24> {
public:
  static const UINT64 mask [24];

  static const UINT64 compare [24];
};

template<int Ways> 
class PLRU_Mask  {

public:
  static inline UINT64 GetMask(int wayNum) {
    return PLRU_MaskInner<Ways>::mask[wayNum];
  }
  static inline UINT64 GetCompare(int wayNum) {
    return PLRU_MaskInner<Ways>::compare[wayNum];
  }
  static UINT64 makeMRU(int way, UINT64 state) {
    return (GetMask(way) & ~GetCompare(way)) | (~GetMask(way) & state);
  }
  static UINT64 makeLRU(int way, UINT64 state) {
    return (GetMask(way) & GetCompare(way)) | (~GetMask(way) & state);
  }
  static int getLRUWayFor(UINT64 state){
    int i;
    for(i=0;i<Ways;i++) {
      if((state & GetMask(i)) == GetCompare(i))
	return i;
    }
    ASSERT(0, "No possible way in LRU selection?");
    return 0;
  }
  static int getLRUWayWithReservations(UINT64 state, UINT64 rsvd) {
    int way;
    int counter = 0;
    do {
      way = getLRUWayFor(state);
      /* if this is reserved, treat it as MRU and try again */
      state = makeMRU(way,state);
      counter++;
      ASSERT(counter<=Ways || ((rsvd & (1<<way)) == 0), "Can't find any ways!");
    }
    while(rsvd & (1 << way));
    return way;
  }
};

#endif 
