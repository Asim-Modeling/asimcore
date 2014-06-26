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

template<int MAXIMUM, int MINIMUM=0>
class ModNumber
{
    //
    //  NOTE:  Internal value representation
    //  
    //  In order to simplify internal calculations, the value will be
    //  represented by 2 numbers: the minimum value and an offset from
    //  the minimum value.
    //
    //  The offset will always be in the range [0..(max_value - min_value)]
    //  Preprocessor macros are used to handle the internal/external and
    //  external/internal mapping to alloc the compiler to optimize for the
    //  common case (minimum value is zero), while avoiding partial template
    //  specialization
    //
  private:
    int offset;
    
    int min_value;
    int max_value;
    unsigned int max_offset;

    //  Check and correct wrapping
    void correctWrap();

  public:
    ModNumber(int initial_value=MINIMUM);

    ModNumber Set(int value);
    ModNumber Set(ModNumber num);

    int Get();

    int GetMax() { return MAXIMUM; }
    int GetMin() { return MINIMUM; }

    ModNumber<MAXIMUM, MINIMUM> Next();
    ModNumber<MAXIMUM, MINIMUM> Prev();

#if OPERATORS
    // a + b == a.operator+(b)
    int operator+(int delta);   
    int operator-(int delta);   
#endif

    //  ++a  == a.operator++()     [prefix]
    ModNumber<MAXIMUM, MINIMUM>  operator++();
    ModNumber<MAXIMUM, MINIMUM>  operator--();

    //  a++  == a.operator++(int)  [postfix]
    ModNumber<MAXIMUM, MINIMUM>  operator++(int dummy);
    ModNumber<MAXIMUM, MINIMUM>  operator--(int dummy);

    //  Booleans
    bool operator==(int value);
    bool operator==(ModNumber num);
    bool operator!=(int value);
    bool operator!=(ModNumber num);
};

//
//  Constructors
//
template<int MAXIMUM, int MINIMUM>
ModNumber<MAXIMUM, MINIMUM>::ModNumber(int initial_value)
{
    min_value = MINIMUM;
    max_value = MAXIMUM;

    max_offset = max_value - min_value;

    offset = MAP_TO_INTERNAL(initial_value);
    correctWrap();
}

template<int MAXIMUM, int MINIMUM>
ModNumber<MAXIMUM, MINIMUM>
ModNumber<MAXIMUM, MINIMUM>::Set(int value)
{
    offset = MAP_TO_INTERNAL(value);
    correctWrap();

    return *this;
}

template<int MAXIMUM, int MINIMUM>
inline ModNumber<MAXIMUM, MINIMUM>
ModNumber<MAXIMUM, MINIMUM>::Set(ModNumber num)
{
    offset = num.offset;
    min_value = num.min_value;
    max_value = num.max_value;

    return *this;
}

template<int MAXIMUM, int MINIMUM>
inline int
ModNumber<MAXIMUM, MINIMUM>::Get()
{
    return MAP_TO_EXTERNAL(offset);
}

template<int MAXIMUM, int MINIMUM>
ModNumber<MAXIMUM, MINIMUM>
ModNumber<MAXIMUM, MINIMUM>::Next()
{
    ++offset;
    correctWrap();

    return *this;
}

//  Prefix ++
template<int MAXIMUM, int MINIMUM>
ModNumber<MAXIMUM, MINIMUM>
ModNumber<MAXIMUM, MINIMUM>::operator++()
{
    ++offset;
    correctWrap();

    return *this;
}

//  Postfix ++
template<int MAXIMUM, int MINIMUM>
ModNumber<MAXIMUM, MINIMUM>
ModNumber<MAXIMUM, MINIMUM>::operator++(int dummy)
{
    ModNumber<MAXIMUM, MINIMUM> rv = *this;

    ++offset;
    correctWrap();

    return rv;
}

//--------------------------------------------

template<int MAXIMUM, int MINIMUM>
ModNumber<MAXIMUM, MINIMUM>
ModNumber<MAXIMUM, MINIMUM>::Prev()
{
    --offset;
    correctWrap();

    return *this;
}

//  Prefix --
template<int MAXIMUM, int MINIMUM>
ModNumber<MAXIMUM, MINIMUM>
ModNumber<MAXIMUM, MINIMUM>::operator--()
{
    --offset;
    correctWrap();

    return *this;
}


//  Postfix --
template<int MAXIMUM, int MINIMUM>
ModNumber<MAXIMUM, MINIMUM>
ModNumber<MAXIMUM, MINIMUM>::operator--(int dummy)
{
    ModNumber<MAXIMUM, MINIMUM> rv = *this;

    --offset;
    correctWrap();

    return rv;
}


#if OPERATORS
template<int MAXIMUM, int MINIMUM>
inline int
ModNumber<MAXIMUM, MINIMUM>::operator+(int delta)
{
}

template<int MAXIMUM, int MINIMUM>
inline int
ModNumber<MAXIMUM, MINIMUM>::operator-(int delta)
{
}
#endif


template<int MAXIMUM, int MINIMUM>
inline bool
ModNumber<MAXIMUM, MINIMUM>::operator==(ModNumber<MAXIMUM, MINIMUM> num)
{
    return (offset == num.offset);
}

template<int MAXIMUM, int MINIMUM>
inline bool
ModNumber<MAXIMUM, MINIMUM>::operator==(int value)
{
    return (offset == MAP_TO_INTERNAL(value));
}

template<int MAXIMUM, int MINIMUM>
inline bool
ModNumber<MAXIMUM, MINIMUM>::operator!=(ModNumber<MAXIMUM, MINIMUM> num)
{
    return (offset != num.offset);
}

template<int MAXIMUM, int MINIMUM>
inline bool
ModNumber<MAXIMUM, MINIMUM>::operator!=(int value)
{
    return (offset != MAP_TO_INTERNAL(value));
}

//========================================

template<int MAXIMUM, int MINIMUM>
inline void
ModNumber<MAXIMUM, MINIMUM>::correctWrap()
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

//========================================

#if 0
template<int MAXIMUM, int MINIMUM>
void 
operator=(int &lvalue, ModNumber<MAX, MIN> &rvalue)
{
    lvalue = rvalue.Get();
}
#endif


#undef MAP_TO_INTERNAL
#undef MAP_TO_EXTERNAL

#endif
