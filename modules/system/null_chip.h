/*****************************************************************************
 *
 * @brief test chip
 *
 * @author Mark Charney
 *
 *  * Copyright (C) 2004-2009 Intel Corporation
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
 *
 *****************************************************************************/

// Container for Common System functions
/**
 * @file
 * @author Sailashri Parthasarathy (based on Chris Weaver's original implementation)
 *
 * @brief The chip for the null pipeline model
 */

#ifndef _CHIP_H_
#define _CHIP_H_

// generic
//#include <time.h>

// ASIM core
#include "asim/syntax.h"
#include "asim/mesg.h"
#include "asim/state.h"
#include "asim/config.h"
#include "asim/stateout.h"

// ASIM public modules
#include "asim/provides/cpu.h"


typedef class ASIM_CHIP_CLASS *ASIM_CHIP;

class ASIM_CHIP_CLASS : public ASIM_MODULE_CLASS 
{
  protected: 
    ASIM_CPU_CLASS** myCpu;

  public:

    ASIM_CHIP_CLASS(ASIM_MODULE parent,                        //CONS
                    const char* const name
                    );

    virtual ~ASIM_CHIP_CLASS();
    bool InitModule() { return true; }

    void DumpStats(STATE_OUT state_out,
                   const UINT64 stat_cycles,
                   const UINT64* committed_insts) { };

    void Clock(const UINT64 cycle);

    // should return the priority for this CPU in a CMP sytem.  just return 0 in UP.
    UINT32 GetHWCPriority(UINT32 cpu_num) { return 0; };
};


#endif 
