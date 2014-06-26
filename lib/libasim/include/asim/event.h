/*****************************************************************************
 *
 * @brief Base class for EVENTS mecanism 
 *
 * @author Roger Gramunt 
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


#ifndef _EVENT_
#define _EVENT_

// ASIM core
#include "asim/syntax.h"
#include "asim/dralServer.h"

using namespace std;

/*
 * If events are not enabled, then no event method invocations will
 * be compiled into the model.
 */
#if defined(ASIM_ENABLE_EVENTS)
#define DRALEVENT(E)          ASIM_DRAL_EVENT_CLASS::event->E
#define DRALEVENT_GUARDED(E)  if (ASIM_DRAL_EVENT_CLASS::event) {ASIM_DRAL_EVENT_CLASS::event->E;}
#define EVENT(...) __VA_ARGS__
#else
#define DRALEVENT(E)
#define DRALEVENT_GUARDED(E)
#define EVENT(...)
#endif

// FEDE: IN ORDER TO AVOID REPLICATING OCCUPANCY STRINGS WE DEFINE THEM HERE
#define DRAL1_STD_OCCUPANCY_TAG "_OCCUPANCY"
#define DRAL2_STD_OCCUPANCY_TAG "OCCUPANCY"

extern bool eventsOn; 
extern bool runWithEventsOn;
extern bool firstTurnedOn;

/*
 * Display ID generation support.
 *
 * We need a GetUniqueDisplayId() function to globally return new, unique
 * display ids to be used in the performance model. This has nothing to do
 * with DRAL, so I've decided to put it here, outside the DRAL definition
 * and implementation classes --- Julio Gago @ BSSAD.
 *
 */
UINT64 GetUniqueDisplayId(void);

/*
 * Class ASIM_EVENT
 *
 * Base class for event management.
 *
 */
typedef class ASIM_DRAL_EVENT_CLASS *ASIM_DRAL_EVENT;
class ASIM_DRAL_EVENT_CLASS 
{
  public:
    // members
    static DRAL_SERVER event;

    // constructors / destructors
    ASIM_DRAL_EVENT_CLASS();
    ~ASIM_DRAL_EVENT_CLASS();

    static void InitEvent();
};

#endif /* _EVENT_ */
