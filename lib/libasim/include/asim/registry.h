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

#ifndef _REGISTRY_
#define _REGISTRY_

// generic
#include <string.h>

// ASIM core
#include "asim/syntax.h"
#include "asim/mesg.h"
#include "asim/resource_stats.h"
#include "asim/stateout.h"
#include "asim/stripchart.h"

typedef class ASIM_STATE_CLASS *ASIM_STATE;
typedef class ASIM_STATELINK_CLASS *ASIM_STATELINK;
typedef class STATE_ITERATOR_CLASS *STATE_ITERATOR;

/*
 * Class ASIM_REGISTRY_CLASS
 *
 * Purpose: Contains all methods which are used to register 
 *          statistics.  This should be the base class for all derived
 *          modules.  Previously, these members and methods resided
 *          within the module class, which made it difficult for
 *          stand-alone code to use the state or event classes.
 * 
 */

typedef class ASIM_REGISTRY_CLASS *ASIM_REGISTRY;
class ASIM_REGISTRY_CLASS
{
  /*
   * The state and event iterators must read the contained modules
   * list and the contained states.
   */
  friend class STATE_ITERATOR_CLASS;
        
private: 
  /*
   * Path passed from module to register package. 
   */
  char *regPath;

protected:   
  ASIM_REGISTRY_CLASS() : regPath(NULL), states(NULL) {}
  virtual ~ASIM_REGISTRY_CLASS();

  /*
   * List of exposed state for which we are registered
   */
  ASIM_STATELINK states;

  /*
   * Method sets the path for the registered stats.  If the path is not 
   * set, it is assumed to be NULL. 
   */
  void SetRegPath(const char *p) {
    ASSERTX (regPath == NULL);
    // +1 for null character ending string.
    regPath = new char[strlen(p) + 1];
    strcpy (regPath, p);
  }

  /*
   * Return the ASIM_STATE object for the state specified by
   * 'stateStr'. Return NULL is no state exists.
   */
  ASIM_STATE SearchStates (const char *stateStr);
  
  /*
   * Dump registered statistics. 
   */
  virtual void DumpStats (STATE_OUT stateOut);

  /*
   * Clear registered statistics. 
   */
  virtual void ClearStats ();

  
public:

  /* 
   * Structure with all the stripchart registered.
   */
  static ASIM_STRIP_CHART_CLASS strip;

  /*
   * Register 'strip_chart' routine
   */
  void RegisterStripChart (const char *description, UINT64 frequency, UINT64 *data, UINT64 threads=THREADS, UINT64 max_elems=0, UINT32 cpunum=UINT32_MAX);
  void HeadDumpStripCharts ();
  void DumpStripCharts (UINT64 cycle);
  void DumpRAWString (char *str);

  /*
   * Register 'state' as an exposed state. 
   */
  ASIM_STATE RegisterState (UINT64 *s, const char * const n,
			    const char * const d, bool sus =true);
  ASIM_STATE RegisterState (UINT64 *s, const UINT32 sz, const char * const n,
			    const char * const d, bool sus =true);
  ASIM_STATE RegisterState (double *s, const char * const n,
			    const char * const d, bool sus =true);
  ASIM_STATE RegisterState (double *s, const UINT32 sz, const char * const n,
			    const char * const d, bool sus =true);
  ASIM_STATE RegisterState (string *s, const char * const n,
			    const char * const d, bool sus =true);

  ASIM_STATE RegisterState (RESOURCE_TEMPLATE<true> *s, 
			    const char * const n, 
			    const char * const d, bool sus = true);
  ASIM_STATE RegisterState (RESOURCE_TEMPLATE<true> *s, 
			    const UINT32 sz, 
			    const char * const n, const char * const d, bool sus = true);
  ASIM_STATE RegisterState (RESOURCE_TEMPLATE<false> *s, 
			    const char * const n, 
			    const char * const d, bool sus = true);
  ASIM_STATE RegisterState (RESOURCE_TEMPLATE<false> *s, 
			    const UINT32 sz, 
			    const char * const n, const char * const d, bool sus = true);

  ASIM_STATE RegisterState (HISTOGRAM_TEMPLATE<true> *s, 
			    const char * const n, 
			    const char * const d, bool sus = true);
  ASIM_STATE RegisterState (HISTOGRAM_TEMPLATE<false> *s, 
			    const char * const n, 
			    const char * const d, bool sus = true);

  ASIM_STATE RegisterState (THREE_DIM_HISTOGRAM_TEMPLATE<true> *s, 
			    const char * const n, 
			    const char * const d, bool sus = true);
  ASIM_STATE RegisterState (THREE_DIM_HISTOGRAM_TEMPLATE<false> *s, 
			    const char * const n, 
			    const char * const d, bool sus = true);



  ASIM_STATE RegisterState (std::vector<UINT64>& s, const UINT32 sz, const char * const n,
			    const char * const d, bool sus =true)
  {
      return RegisterState (&s[0], sz, n, d, sus);
  }
  ASIM_STATE RegisterState (std::vector<double>& s, const UINT32 sz, const char * const n,
			    const char * const d, bool sus =true)
  {
      return RegisterState (&s[0], sz, n, d, sus);
  }
  ASIM_STATE RegisterState (std::vector<RESOURCE_TEMPLATE<false> >& s, 
			    const UINT32 sz, 
			    const char * const n, const char * const d, bool sus = true)
  {
      return RegisterState (&s[0], sz, n, d, sus);
  }

};

#endif /* _REGISTRY_ */
  



