// ==================================================
// Copyright (c) 2014, Intel Corporation
//
// All rights reserved.
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright 
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the distribution.
// - Neither the name of the Intel Corporation nor the names of its 
//   contributors may be used to endorse or promote products derived from 
//   this software without specific prior written permission.
//  
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

/** @file AGTTypes.h
  * @version 0.2
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */


#if !defined(AGT_TYPES)
#define AGT_TYPES

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "agt_syntax.h"

// define some helper methods for 64 bits string to UINT64 conversion and vice-versa
class AGTTypes
{
    public:
        static inline UINT64 str2uint64 (const char *str,bool& parseok)
        {
            UINT64 result;

            int ok=0;
            parseok = false;

            const char *pstr = str;
            while (isspace(*pstr)) ++pstr;
            if ( *pstr == '\0')
            {
                return 0;
            }

            // base 16 ?
            if ((pstr[0]=='0') && (toupper(pstr[1])=='X') )
            {
                ok = sscanf(pstr,FMT64X,&result);
            }

            // base 8 ?
            if ((ok==0) && (pstr[0]=='0') )
            {
                ok = sscanf(pstr,FMT64O,&result);
            }

            // base 10 ?
            if (ok==0)
            {
            ok = sscanf(pstr,FMT64U,&result);
            }
            parseok = (ok>0);
            return (UINT64)result;
        }

        static inline char* uint642str (UINT64 value)
        {
            static char result[32];                    /// @todo This is not thread safe !!
            sprintf(result,FMT64U,value);
            return result;
        }

        static char* bitstring(UINT64 value, int count, int width)
        {
            static char result[128];                      /// @todo This is not thread safe !!
            if (width>127)
            {
                width=127;
            }

            int cnt=0;
            while (--count >= 0)
            {
                result[cnt++] = ((value >> count) & 1) + '0';
            }

            for (int i = cnt; i < width; i++)
            {
                result[cnt++] = ' ';
            }

            result[cnt] = '\0';
            return (result);
        }
};

#endif //!defined(AGT_TYPES)




