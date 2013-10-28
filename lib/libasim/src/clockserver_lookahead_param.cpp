/*
 * **********************************************************************
 *Copyright (C) 2003-2006 Intel Corporation
 *
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
