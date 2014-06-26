/*****************************************************************************
 *
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
 * @author Ramon Matas Navarro
 * @brief Clock edge and phase definitions
 */

#ifndef _PHASE_
#define _PHASE_

#include <iostream>

using namespace std;

typedef enum clk_edge
{
    HIGH = 0,
    LOW,
    NUM_CLK_EDGES
} CLK_EDGE;

static const char* edgesName[NUM_CLK_EDGES] = {"H", "L"};

// -------------------------------------------------------------------------
//
// Class phase

typedef struct phase
{
    UINT64 cycle;
    CLK_EDGE edge;
    
    phase() { }   
    phase(UINT64 _cycle, CLK_EDGE _edge) { cycle = _cycle; edge = _edge; }
    phase(UINT64 phases)
    {
        cycle = phases/2;
        if(phases%2==1) edge = LOW;
        else edge = HIGH;
    }
    
    UINT64 getPhaseNum() const
    {
        return (cycle*2 + edge);
    }

    UINT64 getCycle() const
    {
        return cycle;
    }

    // Operator overloading
    
    friend ostream& operator<< (ostream& os, const phase& phase)
    {
        os << phase.cycle << ':' << edgesName[phase.edge];
        return os;        
    }

    phase& operator=(phase &ph)
    {
        this->cycle = ph.cycle;
        this->edge = ph.edge;
        
        return *this;
    }
    
    bool operator== (const phase ph) const
    {
        return (this->cycle == ph.cycle) && (this->edge == ph.edge);
    }

    bool operator< (const phase ph) const
    {
        return ((this->cycle*2 + this->edge) < (ph.cycle*2 + ph.edge));
    }

    bool operator> (const phase ph) const
    {
        return ((this->cycle*2 + this->edge) > (ph.cycle*2 + ph.edge));
    }    

    bool operator<= (const phase ph) const
    {
        return ((this->cycle*2 + this->edge) <= (ph.cycle*2 + ph.edge));
    }    

    bool operator>= (const phase ph) const
    {
        return ((this->cycle*2 + this->edge) >= (ph.cycle*2 + ph.edge));
    }
        
    phase operator+(const phase &ph) const
    {
        UINT64 phases = this->getPhaseNum() + ph.getPhaseNum();
        phase temp(phases);
                
        return temp;
    }
    
    // + n phases
    phase operator+(const UINT64 n) const
    {
        UINT64 phases = this->getPhaseNum() + n;
        phase temp(phases);
        
        return temp;
    }

    phase operator-(const phase &ph) const
    {
        UINT64 phases = this->getPhaseNum() - ph.getPhaseNum();
        phase temp(phases);
        
        return temp;
    }    
    
    // - n phases
    phase operator-(const UINT64 n) const
    {
        UINT64 phases = this->getPhaseNum() - n;
        phase temp(phases);
        
        return temp;
    }
    
    // + one phase
    phase& operator++()
    {
        if(this->edge == LOW)
        {
            this->edge = HIGH;
            ++this->cycle;
        }
        else
        {
            this->edge = LOW;
        }
        
        return *this;
    }

    phase operator++(int)
    {
        phase temp(this->cycle, this->edge);
        
        if(this->edge == LOW)
        {
            this->edge = HIGH;
            ++this->cycle;
        }
        else
        {
            this->edge = LOW;
        }
        
        return temp;
    }
    
} PHASE;


#endif /* _PHASE_ */
