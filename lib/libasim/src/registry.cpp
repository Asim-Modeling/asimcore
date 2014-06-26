/*
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
 * @author David Goodwin, Srilatha Manne
 * @brief
 */

// generic
#include <sstream>

// ASIM core
#include "asim/registry.h"
#include "asim/state.h"

// ASIM public modules -- BAD! in asim-core
#ifdef CLASSICMODE
#include "asim/provides/controller.h"
#endif

ASIM_STRIP_CHART_CLASS ASIM_REGISTRY_CLASS::strip;

ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (UINT64 *s, const char * const n, 
				    const char * const d, bool sus)
/*
 * Register 'state' as an exposed state owned by this module.
 */
{
    ASIM_STATE ns = new ASIM_STATE_CLASS(s, n, d, regPath, sus);
    states = new ASIM_STATELINK_CLASS(ns, states, true);
    return(ns);
}

ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (UINT64 *s, const UINT32 sz, 
				    const char * const n,
				    const char * const d, bool sus)
{
    ASIM_STATE ns = new ASIM_STATE_CLASS(s, sz, n, d, regPath, sus);
    states = new ASIM_STATELINK_CLASS(ns, states, true);
    return(ns);
}

ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (double *s, const char * const n, 
				    const char * const d, bool sus)
{
    ASIM_STATE ns = new ASIM_STATE_CLASS(s, n, d, regPath, sus);
    states = new ASIM_STATELINK_CLASS(ns, states, true);
    return(ns);
}

ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (double *s, const UINT32 sz, 
				    const char * const n,
				    const char * const d, bool sus)
{
    ASIM_STATE ns = new ASIM_STATE_CLASS(s, sz, n, d, regPath, sus);
    states = new ASIM_STATELINK_CLASS(ns, states, true);
    return(ns);
}

ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (string *s, const char * const n, 
				    const char * const d, bool sus)
{
    ASIM_STATE ns = new ASIM_STATE_CLASS(s, n, d, regPath, sus);
    states = new ASIM_STATELINK_CLASS(ns, states, true);
    return(ns);
}

ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (RESOURCE_TEMPLATE<true> *s,  
				    const char * const n, 
				    const char * const d, bool sus)
{
    ASIM_STATE ns = new ASIM_STATE_CLASS(s, n, d, regPath, sus);
    states = new ASIM_STATELINK_CLASS(ns, states, true);
    return(ns);
}

ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (RESOURCE_TEMPLATE<false> *s,  
				    const char * const n, 
				    const char * const d, bool sus)
{
  return (ASIM_STATE)(NULL);
}

ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (HISTOGRAM_TEMPLATE<true> *s,  
				    const char * const n, 
				    const char * const d, bool sus)
{
    ASIM_STATE ns = new ASIM_STATE_CLASS(s, n, d, regPath, sus);
    states = new ASIM_STATELINK_CLASS(ns, states, true);
    return(ns);
}
    
ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (HISTOGRAM_TEMPLATE<false> *s,  
				    const char * const n, 
				    const char * const d, bool sus)
{
    return (ASIM_STATE)(NULL);
}

ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (THREE_DIM_HISTOGRAM_TEMPLATE<true> *s,  
				    const char * const n, 
				    const char * const d, bool sus)
{
    ASIM_STATE ns = new ASIM_STATE_CLASS(s, n, d, regPath, sus);
    states = new ASIM_STATELINK_CLASS(ns, states, true);
    return(ns);
}
    
ASIM_STATE
ASIM_REGISTRY_CLASS::RegisterState (THREE_DIM_HISTOGRAM_TEMPLATE<false> *s,  
				    const char * const n, 
				    const char * const d, bool sus)
{
    return (ASIM_STATE)(NULL);
}

/**
 * delete all objects we have created
 */
ASIM_REGISTRY_CLASS::~ASIM_REGISTRY_CLASS() {
    if (regPath) {
      delete [] regPath;
    }

    ASIM_STATELINK sscan = states;
    while (sscan != NULL) {
        ASIM_STATELINK tmp = sscan;
        sscan = sscan->next;
        delete tmp;
    }
    states = NULL;
}

/*
 * Purpose: Search through list of states for the requested string. 
 *          If string is found, return the state; otherwise, return 
 *          NULL.
 */

ASIM_STATE
ASIM_REGISTRY_CLASS::SearchStates (const char *stateStr) {

  // Strip leading '/' if there is one.
  if (*stateStr == '/')
    stateStr++;
  
  ASIM_STATELINK sscan = states;
  while (sscan != NULL) {
    ASIM_STATE state = sscan->state;
    if (strncasecmp(state->Name(), stateStr, strlen(state->Name())) == 0)
      return(state);
    
    sscan = sscan->next;
  }
  
  return((ASIM_STATE)(NULL));
}

void
ASIM_REGISTRY_CLASS::DumpStats (STATE_OUT stateOut)
/*
 * Print stats to 'file' for a particular thread.
 */
{
    // This method will likely be overridden by modules to
    // print statistics as they like, however we provide
    // a default action of just printing all the exposed
    // state values.
    
    //YARDI - start
    //fprintf(stderr,"Dumping stats in registry.cpp\n");
    //YARDI - end

    ASIM_STATELINK sscan = states;
    while (sscan != NULL) 
    {
        ASIM_STATE state = sscan->state;

        state->DumpValue(stateOut);

        sscan = sscan->next;
    }
}

void
ASIM_REGISTRY_CLASS::ClearStats ()
/*
 * Clear stats
 */
{

    ASIM_STATELINK sscan = states;
    while (sscan != NULL) 
    {
        ASIM_STATE state = sscan->state;

        state->ClearStats();

        sscan = sscan->next;
    }
}

void
ASIM_REGISTRY_CLASS::RegisterStripChart (const char *description, UINT64 frequency, UINT64 *data, UINT64 threads, UINT64 max_elems, UINT32 cpunum)
{
    strip.RegisterStripChart(description,frequency,data,threads,max_elems,cpunum);
}

void
ASIM_REGISTRY_CLASS::HeadDumpStripCharts ()
{
    strip.HeadDump();
}

void
ASIM_REGISTRY_CLASS::DumpStripCharts (UINT64 cycle)
{
    strip.Dump(cycle);
}

void
ASIM_REGISTRY_CLASS::DumpRAWString(char *str)
{
    strip.DumpRAWString(str);
}
