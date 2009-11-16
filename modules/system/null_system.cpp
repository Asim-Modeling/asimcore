/*****************************************************************************
 *
 * @brief Header file for Null System module
 *
 * @author Carl Beckmann
 *
 *Copyright (C) 2004-2009 Intel Corporation
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
#include <iostream>

// ASIM core
#include "asim/trace.h"
#include "asim/trackmem.h"
#include "asim/cmd.h"
#include "asim/ioformat.h"
#include "asim/event.h"
#include "asim/port.h"

// ASIM public modules
#include "asim/provides/instfeeder_interface.h"
#include "asim/provides/system.h"

/* global_cycle is from mesg.cpp. It is for use by the ASSERT macros. */
extern UINT64 global_cycle;

ASIM_MULTI_CHIP_SYSTEM_CLASS::ASIM_MULTI_CHIP_SYSTEM_CLASS(
    const char *name,
    string reference_domain,
    UINT32 feederThreads
    ) 
    : ASIM_SYSTEM_CLASS(name, 1, NULL, feederThreads)
{
}

ASIM_MULTI_CHIP_SYSTEM_CLASS::~ASIM_MULTI_CHIP_SYSTEM_CLASS()
{
}

void ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_StopPThreads()
{
}
 
bool
ASIM_MULTI_CHIP_SYSTEM_CLASS::InitModule()
{
    return false;
}

void
SYS_Usage(FILE *file)
{    
}

void
ASIM_MULTI_CHIP_SYSTEM_CLASS::DumpStats(STATE_OUT state_out)
{
}

bool
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_ScheduleThread(
    ASIM_THREAD thread)
{
    return false;
}

bool
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_UnscheduleThread(
    ASIM_THREAD thread)
{
    return false;
}

bool
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_BlockThread(
    ASIM_THREAD thread, 
    ASIM_INST inst)
{
    return false;
}

bool
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_UnblockThread(
    ASIM_THREAD thread)
{
    return false;
}

bool
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_HookAllThreads()
{
    return false;
}

bool
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_UnhookAllThreads()
{
    return false;
}

bool
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_IsCpuActive(UINT32 cpunum) const
{
    return false;
}

bool
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_Execute(
    const UINT64 stop_nanosecond,
    const UINT64 stop_cycle, 
    const UINT64 stop_inst, 
    const UINT64 stop_macroinst,
    const UINT64 stop_packet)
{
    return false;
}

void 
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_Break()
{
}

void 
ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_ClearBreak()
{
}

ASIM_SYSTEM
SYS_Init(
    UINT32 argc, 
    char *argv[], 
    char *envp[],
    UINT32 feederThreads)
{
    string reference_domain = "";
    return new ASIM_MULTI_CHIP_SYSTEM_CLASS("COMMON_SYSTEM",
                                            reference_domain,
                                            feederThreads);
}

UINT64 ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_Cycle (UINT32 cpunum)
{
    return 0;
}    

UINT64& ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_Cycle (void)
{
    return global_cycle;
}    

UINT64 ASIM_MULTI_CHIP_SYSTEM_CLASS::SYS_Nanosecond (void)
{
    return 0;
}
   
CONTEXT_SCHEDULER ASIM_MULTI_CHIP_SYSTEM_CLASS::GetContextScheduler(void)
{
    return NULL;
}

WARMUP_MANAGER ASIM_MULTI_CHIP_SYSTEM_CLASS::GetWarmupManager(void)
{
    return NULL;
}
