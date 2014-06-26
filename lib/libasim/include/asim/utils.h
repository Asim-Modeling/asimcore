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

// Partly from the Wisconsin Wind Tunnel simulator.  -Shubu

#ifndef _UTILS_
#define _UTILS_

// ASIM core
#include "asim/syntax.h"

// insanity here you come: 
// CEIL_LOG2(a): preprocessor macro for computing the
//               smallest power of two >= a; works by using ...
// LEADING_ZEROS(a): preprocessor macro for computing the number
//                   of leading 0-bits in a 64-bit word;
//                   complexity is O((log n)^2), n .. # bits in word
#define LDZ_TOP_HALF(a,b) (((a) >= (1LL << (1L << (b)))) << (b))
#define LDZ_A5(a) (a)
#define LDZ_A4(a) (LDZ_A5(a) >> LDZ_TOP_HALF(LDZ_A5(a),5))
#define LDZ_A3(a) (LDZ_A4(a) >> LDZ_TOP_HALF(LDZ_A4(a),4))
#define LDZ_A2(a) (LDZ_A3(a) >> LDZ_TOP_HALF(LDZ_A3(a),3))
#define LDZ_A1(a) (LDZ_A2(a) >> LDZ_TOP_HALF(LDZ_A2(a),2))
#define LDZ_A0(a) (LDZ_A1(a) >> LDZ_TOP_HALF(LDZ_A1(a),1))
#define LDZ_RET5(a) (LDZ_TOP_HALF((a),5))
#define LDZ_RET4(a) (LDZ_RET5(a) + LDZ_TOP_HALF(LDZ_A4(a),4))
#define LDZ_RET3(a) (LDZ_RET4(a) + LDZ_TOP_HALF(LDZ_A3(a),3))
#define LDZ_RET2(a) (LDZ_RET3(a) + LDZ_TOP_HALF(LDZ_A2(a),2))
#define LDZ_RET1(a) (LDZ_RET2(a) + LDZ_TOP_HALF(LDZ_A1(a),1))
#define LDZ_RET0(a) (LDZ_RET1(a) + LDZ_TOP_HALF(LDZ_A0(a),0))

#define LEADING_ZEROS(a) (((a) == 0) ? 64 : 63 - LDZ_RET0(a))
#define FLOOR_LOG2(a) ((a) == 0 ? 0 : (64 - LEADING_ZEROS(a) - 1))
#define FLOOR_POW2(a) ((a) == 0 ? 0 : (1L << FLOOR_LOG2(a)))
#define CEIL_LOG2(a) ((a) == 0 ? 0 : (64 - LEADING_ZEROS((a)-1)))
#define CEIL_POW2(a) ((a) == 0 ? 0 : (1L << CEIL_LOG2(a)))

// floor(log2(n))... works by finding position of MSB set.
// Returns -1 if n == 0.
inline INT32
floorLog2(UINT32 n)
{
    INT32 p = 0;

    if (n == 0)
    {
	return -1;
    }

    if (n & 0xffff0000) { p += 16; n >>= 16; }
    if (n & 0x0000ff00)	{ p +=  8; n >>=  8; }
    if (n & 0x000000f0) { p +=  4; n >>=  4; }
    if (n & 0x0000000c) { p +=  2; n >>=  2; }
    if (n & 0x00000002) { p +=  1; }

    return p;
}

//log base 2 of argument... actually returns floor(log2(n)).
inline INT32
ilog2(UINT32 n)
{ return floorLog2(n); }

// ceil(n/d)
inline INT32
divCeil(INT32 n, INT32 d)
{ return (n + d - 1) / d; }

inline INT32
ceilLog2(UINT32 n)
{ return floorLog2(n-1)+1; }

inline INT32
floorPow2(UINT32 n)
{ return 1 << floorLog2(n); }

inline INT32
ceilPow2(UINT32 n)
{ return 1 << ceilLog2(n); }

inline INT32
isPowerOf2(UINT32 n)
{ return ceilLog2(n) == floorLog2(n); }

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
inline UINT32
leadingZeros(UINT64 a) {
  // GEM CC intrinsic; really fast if compiled with "-tune ev67 -intrinsics"
  return _leadz(a);
}

//
// compute the number of trailing zeros
//
inline UINT32
trailingZeros(UINT64 a) {
  // GEM CC intrinsic; really fast if compiled with "-tune ev67 -intrinsics"
  return _trailz(a);
}
#else
UINT32 leadingZeros(UINT64 a);
UINT32 trailingZeros(UINT64 a);
#endif // __DECC

char *strjoin(char *s1, char *s2);

#endif // !_UTILS_

