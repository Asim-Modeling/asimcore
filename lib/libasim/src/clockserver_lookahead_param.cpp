/*
 * **********************************************************************
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
 * @author Carl Beckmann
 **/

#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <sched.h>
#include <string>

#include "asim/clockserver.h"
#include "asim/clockable.h"
#include "asim/module.h"
#include "asim/rate_matcher.h"


//
// Parse the parameter string and set the fuzy barrier lookahead.
// The format of the input string is:
//
//        cycles
// or     domainname:cycles
//
// where "cycles" is a floating-point number.
// This translates the number of cycles of the
// given clock domain (or the default clock domain)
// into base frequency cycles for internal use.
//
UINT64 ASIM_CLOCK_SERVER_CLASS::LookaheadParam2BaseCycles( const string &lookahead )
{
    float cycles        = 0.0;
    CLOCK_DOMAIN domain = NULL;

    size_t colon_loc = lookahead.find(":");
    if ( colon_loc == string::npos )
    {
        cycles = atof( lookahead.c_str() );
        // no domain name given?  use default clock domain
        domain = referenceClockDomain;
        if ( domain == NULL )
        {
            domain = lDomain.front();
            cerr << "WARNING!  Reference clock domain not set yet, using "
                 << domain->name << " for lookahead calculation!\n";
        }
    }
    else
    {
        cycles = atof( lookahead.substr( colon_loc+1 ).c_str() );
        // extract domain name, and find the clock domain
        string domain_name = lookahead.substr( 0, colon_loc );
        list<CLOCK_DOMAIN>::iterator iter_dom = lDomain.begin();
        for ( ; iter_dom != lDomain.end() && domain == NULL; ++iter_dom )
        {
            if ( (*iter_dom)->name == domain_name )
            {
                domain = (*iter_dom);
            }
        }
        if ( domain == NULL )
        {
            // print a helpful message if the domain name wasn't found:
            cerr << "Clock domain " << domain_name << " not found!\nTry one of:\n";
            for ( iter_dom = lDomain.begin(); iter_dom != lDomain.end(); ++iter_dom )
            {
                cerr << (*iter_dom)->name << endl;
            }
        }
    }

    // translate the given clock domain and number of cycles
    // into an integer number of base frequency cycles
    UINT64 domain_base_cycles = domain->lClock.front()->nStep;
    UINT64 lookahead_cycles = cycles * domain_base_cycles;
    
    // DEBUG
    cout << "Setting lookahead, domain=" << domain->name << ", cycles="
         << cycles << " * " << domain_base_cycles << " = " << lookahead_cycles << endl;

    return lookahead_cycles;
}
