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

/**
 * @file
 * @author Shubu Mukherjee, Artur Klauser
 * @brief Usefull little gadgets.
 */

// generic
#include <cstring>

// ASIM core
#include "asim/utils.h"
#include "asim/syntax.h"
#include "asim/mesg.h"

char *
strjoin(char *s1, char *s2)
{
 INT32 len;
 char *ptr;

 ASSERTX(s1 != NULL);
 ASSERTX(s2 != NULL);
 len = strlen(s1) + strlen(s2) + 1;
 ptr = new char[len];

 strcpy(ptr,s1);
 strcat(ptr,s2);

 return ptr;
}


//
// DEC CC/CXX support fast intrinsic trailing/leading zeros implementations;
// other compilers have to use our own C code
//
#ifdef __DECC

// generic
#include <alpha/builtins.h>

//
// compute the number of leading zeros
//
UINT32 leadingZeros(UINT64 a) {
  // GEM CC intrinsic; really fast if compiled with "-tune ev67 -intrinsics"
  return _leadz(a);
}

//
// compute the number of trailing zeros
//
UINT32 trailingZeros(UINT64 a) {
  // GEM CC intrinsic; really fast if compiled with "-tune ev67 -intrinsics"
  return _trailz(a);
}

#else // __DECC

//
// compute the number of leading zeros
//
UINT32 leadingZeros(UINT64 a) {
  UINT32 ret;
  UINT32 topHalf;

  if (a == 0) {
    return 64;
  }

  //--------------------------------
  topHalf = (a >= (1LL << 32)) << 5;
  ret = topHalf;
  a >>= topHalf;
  //--------------------------------
  topHalf = (a >= (1L << 16)) << 4;
  ret += topHalf;
  a >>= topHalf;
  //--------------------------------
  topHalf = (a >= (1L << 8)) << 3;
  ret += topHalf;
  a >>= topHalf;
  //--------------------------------
  topHalf = (a >= (1L << 4)) << 2;
  ret += topHalf;
  a >>= topHalf;
  //--------------------------------
  topHalf = (a >= (1L << 2)) << 1;
  ret += topHalf;
  a >>= topHalf;
  //--------------------------------
  topHalf = (a >= (1L << 1)) << 0;
  ret += topHalf;
  //--------------------------------

  return (63 - ret);
}

//
// compute the number of trailing zeros
//
UINT32 trailingZeros(UINT64 a) {
  UINT32 ret;
  UINT32 notBottomHalf;

  if (a == 0) {
    return 64;
  }

  //--------------------------------
  notBottomHalf = ((a & ((1LL << 32) - 1)) == 0) << 5;
  ret = notBottomHalf;
  a >>= notBottomHalf;
  //--------------------------------
  notBottomHalf = ((a & ((1L << 16) - 1)) == 0) << 4;
  ret += notBottomHalf;
  a >>= notBottomHalf;
  //--------------------------------
  notBottomHalf = ((a & ((1L << 8) - 1)) == 0) << 3;
  ret += notBottomHalf;
  a >>= notBottomHalf;
  //--------------------------------
  notBottomHalf = ((a & ((1L << 4) - 1)) == 0) << 2;
  ret += notBottomHalf;
  a >>= notBottomHalf;
  //--------------------------------
  notBottomHalf = ((a & ((1L << 2) - 1)) == 0) << 1;
  ret += notBottomHalf;
  a >>= notBottomHalf;
  //--------------------------------
  notBottomHalf = ((a & ((1L << 1) - 1)) == 0) << 0;
  ret += notBottomHalf;
  //--------------------------------

  return (ret);
}

#endif // __DECC
