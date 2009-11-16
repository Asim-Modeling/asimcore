/*****************************************************************************
 *
 * @brief Header file for Null system module
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
#ifndef __NULL_SYSTEM__
#define __NULL_SYSTEM__

// generic
#include <time.h>

// ASIM core
#include "asim/syntax.h"
#include "asim/mesg.h"
#include "asim/state.h"
#include "asim/config.h"
#include "asim/stateout.h"
#include "asim/smp.h"

// ASIM public modules
#include "asim/provides/basesystem.h"

void RegisterSimulatorConfiguration(ASIM_REGISTRY reg);


typedef class ASIM_MULTI_CHIP_SYSTEM_CLASS* ASIM_MULTI_CHIP_SYSTEM;

class ASIM_MULTI_CHIP_SYSTEM_CLASS : public ASIM_SYSTEM_CLASS 
{
  public:
  
    ASIM_MULTI_CHIP_SYSTEM_CLASS(
        const char *name,
        string reference_domain,
        UINT32 feederThreads);
    virtual ~ASIM_MULTI_CHIP_SYSTEM_CLASS();

    virtual void DumpStats(STATE_OUT state_out);

    bool InitModule();

    // Interface functions. The following functions represent
    // a performance models external interface.
    friend void SYS_Usage(FILE * file);
    bool SYS_ScheduleThread(ASIM_THREAD thread);
    bool SYS_UnscheduleThread(ASIM_THREAD thread);
    bool SYS_BlockThread(ASIM_THREAD thread, ASIM_INST inst);
    bool SYS_UnblockThread(ASIM_THREAD thread);
    bool SYS_HookAllThreads();
    bool SYS_IsCpuActive(UINT32 cpunum) const; 
    bool SYS_UnhookAllThreads();
    bool SYS_Execute(const UINT64 stop_nanosecond, const UINT64 stop_cycle, const UINT64 stop_inst, const UINT64 stop_macroinst, const UINT64 stop_packet=0);
    void SYS_Break();
    void SYS_ClearBreak();
    void SYS_StopPThreads();
    
    // We need to redefine the following function
    UINT64 SYS_Cycle (UINT32 cpunum);
    UINT64& SYS_Cycle (void);
    UINT64 SYS_Nanosecond (void);
   
    CONTEXT_SCHEDULER GetContextScheduler(void);
    WARMUP_MANAGER GetWarmupManager(void);
};


#endif // __NULL_SYSTEM__
