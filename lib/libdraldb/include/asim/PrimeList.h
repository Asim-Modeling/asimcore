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

#ifndef _DRALDB_PRIMELIST_H
#define _DRALDB_PRIMELIST_H

#include "asim/draldb_syntax.h"

/**
  * @brief
  * This class is used to find primes near a value.
  *
  * @description
  * The only content in this class is a static function that search
  * the first bigger prime in the list. All the primes are defined
  * in a list inside the function.
  *
  * @link http://www.mathematical.com/primelist1to100kk.html
  * @author Federico Ardanaz
  * @date started at 2002-12-1
  * @version 0.1
  */
class PrimeList
{
    public:
       static inline INT32 nearPrime(INT32 size);
};

/**
 * This function finds a prime that is bigger than the parameter
 * size. If no bigger prime is found a 9999991 is returned.
 *
 * @return a near primer.
 */
INT32
PrimeList::nearPrime(INT32 size)
{
    INT32 primeArray[] =
    {
        17,59,97,151,197,257,317,419,521,617,709,811,911,
        1009,1499,2003,2503,3001,3499,
        4001,4507,5003,5501,6007,6521,
        7001,7499,8009,8501,9001,9497,
        10007,20011,30011,
        40009,50021,60013,
        70001,80021,90001,
        100003,150001,
        200003,250007,
        300089,350003,
        400009,450001,
        500009,550007,
        600011,650011,
        700001,750019,
        800011,850009,
        900001,950009,
        1000003,1200007,1300021,
        1400017,1500007,1600033,
        1700021,1800017,1900009,
        2000003,
        3000017,
        4500007,4600003,4700021,4800007,4900001,
        5000011,5500003,
        6000011,
        7000003,
        8000009,
        9000011,
        9900047,
        -1
    };
    INT32 result = -1;
    bool fnd = false;
    INT32 idx = 0;

    while (!fnd && primeArray[idx]>0)
    {
        fnd = primeArray[idx]>=size;
        ++idx;
    }
    if (fnd)
    {
        result = primeArray[idx-1];
    }
    else
    {
        // a very big one!
        result = 9999991;
    }

    //printf ("nearPrime called width %d result=%d\n",size,result);fflush(stdout);
    return (result);
}

#endif
