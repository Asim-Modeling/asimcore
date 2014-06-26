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

#ifndef MOD_NUMBERS_H
#define MOD_NUMBERS_H

#define MAP_TO_INTERNAL(x) (int)((x) - min_value)
#define MAP_TO_EXTERNAL(x) (int)((x) + min_value)
class ModNumberDyn
{
  private:
    int offset;
    
    int min_value;
    int max_value;
    unsigned int max_offset;

    //  Check and correct wrapping
    void correctWrap()
    {
        //  Since the correct 'offset' value is always in the
        //  range [0..(max_value - min_value)], we can simply
        //  use the modulo operator to correct wrapping problems

        //  correct the 'past the high end' wrap
        offset = offset % (max_offset + 1);

        //  correct for the 'lower than the low end' wrap
        if (offset < 0) {
            offset = offset + (max_offset + 1);
        }
    }

  public:
    // default sets up a toggler
    ModNumberDyn(int max=1, int min=0, int value=0)
    {
        SetMaxMin(max, min, value);
    }

    ModNumberDyn(const ModNumberDyn & x, int value)
    {
        *this = x;
        Set(value);
    }

    ModNumberDyn Set(int value)
    {
        offset = MAP_TO_INTERNAL(value);
        correctWrap();
        
        return *this;
    }

    ModNumberDyn Set(ModNumberDyn num)
    {
        offset = num.offset;
        min_value = num.min_value;
        max_value = num.max_value;
        
        return *this;
    }
    
    int Get()
    {
        return MAP_TO_EXTERNAL(offset);
    }

    int GetMax() 
    { 
        return max_value; 
    }
    
    int GetMin() 
    { 
        return min_value; 
    }

    void SetMaxMin(int max=1, int min=0, int initial_value=0)
    {
        // asim bug #0000741.  do not allow for max to be less than the min.  
        // this can cause very bad things to happen (see bug report).
        // --slechta
        if (max < min)
        {
            max = min;
        }

        min_value = min;
        max_value = max;
        
        max_offset = max_value - min_value;
        
        offset = MAP_TO_INTERNAL(initial_value);
        correctWrap();
    }

    ModNumberDyn Next()
    {
        ++offset;
        correctWrap();
        
        return *this;
    }

    ModNumberDyn Prev()
    {
        --offset;
        correctWrap();
        
        return *this;
    }

#if OPERATORS
    // a + b == a.operator+(b)
    int operator+(int delta) {}
    int operator-(int delta) {}
#endif

    //  ++a  == a.operator++()     [prefix]
    ModNumberDyn operator++()
    {
        ++offset;
        correctWrap();

        return *this;
    }

    ModNumberDyn operator--()
    {
        --offset;
        correctWrap();

        return *this;
    }

    //  a++  == a.operator++(int)  [postfix]
    ModNumberDyn operator++(int dummy)
    {
        ModNumberDyn rv(*this);

        ++offset;
        correctWrap();

        return rv;
    }

    ModNumberDyn operator--(int dummy)
    {
        ModNumberDyn rv(*this);

        --offset;
        correctWrap();

        return rv;
    }

    //  Booleans
    bool operator==(int value)
    {
        return (offset == (int)(MAP_TO_INTERNAL(value) % (max_offset + 1)));
    }

    bool operator==(ModNumberDyn num)
    {
        return (offset == num.offset);
    }

    bool operator!=(int value)
    {
        return (offset != (int)(MAP_TO_INTERNAL(value) % (max_offset + 1)));
    }

    bool operator!=(ModNumberDyn num)
    {
        return (offset != num.offset);
    }
};



#undef MAP_TO_INTERNAL
#undef MAP_TO_EXTERNAL

#endif
