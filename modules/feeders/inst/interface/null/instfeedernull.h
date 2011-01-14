/**************************************************************************
 *Copyright (C) 2002-2009 Intel Corporation
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
 **************************************************************************/

/**
 * @file
 * @author Artur Klauser, Carl Beckmann
 * @brief NULL Instruction feeder interface definition.
 */

#ifndef _INSTFEEDERNULL_
#define _INSTFEEDERNULL_

#include "asim/stateout.h"
#include "asim/provides/iaddr.h"

//----------------------------------------------------------------
// Define minimal set of what the controller expects to get defined
//----------------------------------------------------------------

typedef int IFEEDER_STREAM_HANDLE;

// instruction feeder base class
class IFEEDER_BASE_CLASS {
  public:
    enum ASIM_MARKER_CMD {
        MARKER_CLEAR_PC = 0,
        MARKER_CLEAR_ALL,
        MARKER_SET_PC,
        MARKER_SET_INST
    };

    bool Init(UINT32 argc, char **argv, char **envp)        { return true; }
    UINT32 NActiveThreads(void) const                       { return 0;     }
    void Marker(ASIM_MARKER_CMD cmd,
                IFEEDER_STREAM_HANDLE stream,
                UINT32 markerID,
                IADDR_CLASS markerPC = IADDR_CLASS(),
                UINT32 instBits = 0,
                UINT32 instMask = 0)                        {               }
    UINT64 Skip(IFEEDER_STREAM_HANDLE stream,
                UINT64 n,
                INT32 markerID = -1)                        { return 0;     }
    UINT64 Symbol(IFEEDER_STREAM_HANDLE stream, char* name) { return 0;     }
    UINT64 Symbol(char *name)                               { return 0;     }
    char *Symbol_Name(IFEEDER_STREAM_HANDLE stream,
                      UINT64 address,
                      UINT64& offset)                       { return NULL;  }

    static UINT64 SingleFeederSymbolHack(char *name)        { return 0;     }
    static void AllDone(void)                               {               }
    static void DeleteAllFeeders(void)                      {               }
    static void DumpAllFeederStats(STATE_OUT state_out)     {               }
    static void ClearAllFeederStats()                       {               }
};
typedef IFEEDER_BASE_CLASS *IFEEDER_BASE;

// a thread in a benchmark trace
class ASIM_THREAD_CLASS {
  public:
    IFEEDER_BASE IFeeder(void) const                        { return NULL;  }
    IFEEDER_STREAM_HANDLE IStreamHandle(void) const         { return 0;     }
    UINT32 TUid(void) const                                 { return 0;     }
};
typedef ASIM_THREAD_CLASS *ASIM_THREAD;

// the instruction feeder implementation normally provides these routines
inline IFEEDER_BASE IFEEDER_New(void)                       { return NULL;  }
inline void IFEEDER_Usage(FILE *file)                       {               }

#endif /* _INSTFEEDERNULL_ */
