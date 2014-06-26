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

#include "asim/event.h"
#include "asim/smp.h"

// what is this for?
bool eventsOn = false;

bool runWithEventsOn = false;

bool firstTurnedOn = true;

DRAL_SERVER ASIM_DRAL_EVENT_CLASS::event;

// a global object that can destruct things at program termination
ASIM_DRAL_EVENT_CLASS dralCleaner;

/**
 * initialize variables
 */
ASIM_DRAL_EVENT_CLASS::ASIM_DRAL_EVENT_CLASS()
{
    event = NULL;
}

/**
 * free allocated memory
 */
ASIM_DRAL_EVENT_CLASS::~ASIM_DRAL_EVENT_CLASS()
{
    if (event)
    {
        delete event;
    }
    event = NULL;
}

void
ASIM_DRAL_EVENT_CLASS::InitEvent()
{
    runWithEventsOn = true;

    event = new DRAL_SERVER_CLASS("Events",1024,true);
    //eventsOn = false;
    // Send an event creating 
    // a fake node with number 0. This is necessary because port.h will connect to node 0
    // those ports whose origin or destination is unknown to "Fake Node".
    event->NewNode("Fake_Node", 0);
}

/*
 * Display ID generation support.
 */
UINT64
GetUniqueDisplayId(void)
{
    static UINT64 displayId = 1;
    
    return displayId++;
} 
