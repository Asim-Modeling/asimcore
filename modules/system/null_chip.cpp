/*****************************************************************************
 *
 * @brief Chip
 *
 * @author Sailashri Parthasarathy (based on Mark Charney's original implementation)
 *
 * Copyright (C) 2004-2009 Intel Corporation
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

// generic
#include <signal.h>
#include <unistd.h>
#include <sstream>

// ASIM core
#include "asim/trace.h"
#include "asim/trackmem.h"
#include "asim/cmd.h"
//#include "asim/ioformat.h"

// ASIM public modules
#include "asim/provides/instfeeder_interface.h"
#include "asim/provides/chip.h"


ASIM_CHIP_CLASS::ASIM_CHIP_CLASS(
    ASIM_MODULE parent,             // CONS
    const char* const name)
    : ASIM_MODULE_CLASS(parent, name)
{
    NewClockDomain("CORE_CLOCK_DOMAIN", (float) 4);

    myCpu = new ASIM_CPU_CLASS*[TOTAL_NUM_CPUS];
    for(int i = 0; i < TOTAL_NUM_CPUS; i++)
    {
        myCpu[i] = new ASIM_CPU_CLASS(this, "CPU",i);
    }
}

ASIM_CHIP_CLASS::~ASIM_CHIP_CLASS()
{
    for(int i = 0; i < TOTAL_NUM_CPUS; i++)
    {
        delete myCpu[i];
    }
    delete[] myCpu;
}


void 
ASIM_CHIP_CLASS::Clock(
    const UINT64 cycle)
{
    for(int i = 0; i < TOTAL_NUM_CPUS; i++)
    {
        myCpu[i]->Clock(cycle);
    }
}

