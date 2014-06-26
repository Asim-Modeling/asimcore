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
 * @file register.h
 * @author Michael Adler
 * @brief Architecture neutral register properties
 */

// ASIM core
#include "asim/syntax.h"
#include "asim/mesg.h"
#include "asim/arch_register.h"

ostream&
operator<< (
    ostream& out,
    const ARCH_REGISTER_CLASS &reg)
{
    if (! reg.IsValid())
    {
        out << "<Invalid>";
        return out;
    }

    char t;
    switch (reg.GetType())
    {
      case REG_TYPE_INT:
        t = 'I';
        break;
      case REG_TYPE_FP64:
      case REG_TYPE_FP82:
        t = 'F';
        break;
      case REG_TYPE_PREDICATE:
      case REG_TYPE_PREDICATE_VECTOR64:
        t = 'P';
        break;
      case REG_TYPE_AR:
        t = 'A';
        break;
      case REG_TYPE_BR:
        t = 'B';
        break;
      case REG_TYPE_CR:
        t = 'C';
        break;
      case REG_TYPE_MSR:
        t = 'M';
        break;
      case REG_TYPE_SEG:
        t = 'S';
        break;
      case REG_TYPE_RAW:
        t = 'R';
        break;
      default:
        out << "<Unknown reg type>";
        return out;
    }

    out << t;
    if (reg.GetType() == REG_TYPE_PREDICATE_VECTOR64)
    {
        out << 'V';
    }
    else
    {
        out << reg.GetNum();
    }

    out << " (";

    if (! reg.HasKnownValue())
    {
        out << "<Unknown value>";
    }
    else
    {
        if (reg.GetType() == REG_TYPE_RAW)
        {
            out << "0x";
            UINT32 i = reg.GetRawSize();
            const UINT8 *value = (const UINT8 *)reg.GetRawValue();
            while (i)
            {
                i -= 1;
                out << fmt("02x", UINT32(value[i]));
            }
        }
        else if (reg.GetType() == REG_TYPE_FP82)
        {
            out << reg.GetFP82Sign()
                << " 0x" << fmt_x(reg.GetFP82Exponent())
                << " 0x" << fmt_x(reg.GetFP82Significand())
                << " [bignum]";
        }
        else
        {
            out << "0x" << fmt_x(reg.GetIValue()) << dec;

            if (reg.GetType() == REG_TYPE_FP64)
            {
                out << " [" << reg.GetFValue() << "]";
            }
        }
    }

    out << ")";
    return out;
}
