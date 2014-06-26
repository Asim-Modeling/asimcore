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
 * @author Ross Dickson
 * @brief Fundamental data type for addresses
 */

#ifndef _ADDRESS_
#define _ADDRESS_

// ASIM core
#include "asim/syntax.h"
#include "asim/utils.h"

// We can't use UINT64_MAX because the feeder seems to output UINT64_MAX
// as a bad value
#define UINIT_VALUE	(UINT64_MAX-1)

typedef class ADDRESS_CLASS *ADDRESS;
class ADDRESS_CLASS
{
private:
  UINT64 	VirtualAddr;		// virtual address initialized by feeder
  bool		isVirtualAddrSet;	// have we set this variable? 
  
  UINT64  	pid;            	// process or address space ID, initialized by feeder
  bool		isPidSet;		// have we set the pid?

  UINT64 	RealPhysicalAddr;	// physical address initialized by feeder
  bool	        isRealPhysicalAddrSet;  // have we set the real physical address? 

  UINT64 	SimPhysicalAddr; 	// 64-bit simulated physical address used by asim
  bool		isSimPhysicalAddrSet; 	// have set simulate

  UINT64	BLOCKADDR(UINT64 v, INT32 l) const { return (v >> l); }

  int  		mapCode; 		// code used to determine mapping function for SimPhysicalAddr

public:
  ADDRESS_CLASS();

  ADDRESS_CLASS(int mc);

  ADDRESS_CLASS(int mc, UINT64 v, UINT64 pi, UINT64 physicalAddress); 

  // Creates the SimPhysicalAddr based on a combination of VA, Pid, and PA
  UINT64 MapToSimPhysical(int mc)
  { 
      switch(mc)
      {
	case 0:
	  return RealPhysicalAddr;

	case 1:
	  return ((pid << 42) + VirtualAddr);

	case 2:
	  return VirtualAddr;

	default:
	  ASSERTX(0); 
	  return 0; 
      }
  }

  // Accessors
  UINT64   VEA() const		{ ASSERTX(isVirtualAddrSet && mapCode != -1);		return VirtualAddr; }
  UINT64   OracleVEA() const	{ return VirtualAddr; }
  UINT64   Pid() const		{ ASSERTX(isPidSet && mapCode != -1); 			return pid; }
  UINT64   RealPA() const	{ ASSERTX(isRealPhysicalAddrSet && mapCode != -1); 	return RealPhysicalAddr; }
  UINT64   PA() const		{ ASSERTX(isSimPhysicalAddrSet && mapCode != -1);	return SimPhysicalAddr; }

  UINT64   BlockAddr(INT32 log2BlockBytes) const 
  { 
      return BLOCKADDR(SimPhysicalAddr, log2BlockBytes); 
  }

  void SetMapCode(int mc)
  {	
      mapCode = mc; 
      SimPhysicalAddr = MapToSimPhysical(mapCode); 
      isSimPhysicalAddrSet = true; 
  }

  // Modifiers
  void SetAddress(int mc, UINT64 v, UINT64 pi, UINT64 p)
  {
      mapCode = mc; 

      VirtualAddr = v;	
      isVirtualAddrSet = true;

      pid = pi;
      isPidSet = true;

      RealPhysicalAddr = p;
      isRealPhysicalAddrSet = true;

      SimPhysicalAddr = MapToSimPhysical(mc); 
      isSimPhysicalAddrSet = true; 
  }

  void SetVEA(UINT64 v)	
  { 
      VirtualAddr = v; 	
      isVirtualAddrSet = true; 	

      SimPhysicalAddr = MapToSimPhysical(mapCode); 
      isSimPhysicalAddrSet = true; 
  }

  void OracleSetVEA(UINT64 v)	
  { 
      VirtualAddr = v; 	

      // not sure if we need this or not.
      //      SimPhysicalAddr = MapToSimPhysical(mapCode); 
  }

  void SetPid(UINT64 p)	
  { 
      pid = p; 		
      isPidSet = true; 		
      SimPhysicalAddr = MapToSimPhysical(mapCode); 
      isSimPhysicalAddrSet = true;       
  } 

  void SetRealPA(UINT64 p)	
  {
      RealPhysicalAddr = p; 
      isRealPhysicalAddrSet = true;
      SimPhysicalAddr = MapToSimPhysical(mapCode); 
      isSimPhysicalAddrSet = true;       
  }

  // comparators

  bool MatchBlockV(const ADDRESS_CLASS &a, INT32 log2BlockBytes) const {
    ASSERTX(mapCode != -1); 
    return (BLOCKADDR(VirtualAddr, log2BlockBytes) == 
	    BLOCKADDR(a.VEA(), log2BlockBytes));
  }

  bool MatchBlockP(const ADDRESS_CLASS &a, INT32 log2BlockBytes) const {
    ASSERTX(mapCode != -1); 
    return (BLOCKADDR(SimPhysicalAddr, log2BlockBytes) == 
	    BLOCKADDR(a.PA(), log2BlockBytes));
  }

  bool MatchBlockP(UINT64 a, INT32 log2BlockBytes) const {
    ASSERTX(mapCode != -1); 
    return (BLOCKADDR(SimPhysicalAddr, log2BlockBytes) == 
	    BLOCKADDR(a, log2BlockBytes));
  }

  bool MatchVEAddr(const ADDRESS_CLASS &a) const {
    ASSERTX(mapCode != -1); 
    return (VirtualAddr == a.VEA());
  }

  bool MatchPAddr(const ADDRESS_CLASS &a) const {
    ASSERTX(mapCode != -1); 
    return (SimPhysicalAddr == a.PA());
  }

};


inline
ADDRESS_CLASS::ADDRESS_CLASS(int mc) {
  mapCode = mc;

  VirtualAddr = pid = RealPhysicalAddr = SimPhysicalAddr = UINIT_VALUE;
  isVirtualAddrSet = isPidSet = isRealPhysicalAddrSet = isSimPhysicalAddrSet = false; 
}

inline
ADDRESS_CLASS::ADDRESS_CLASS() {
  ADDRESS_CLASS(-1); 
}

inline
ADDRESS_CLASS::ADDRESS_CLASS(int mc, UINT64 v, UINT64 pi, UINT64 physicalAddress) 
{
    SetAddress(mc, v, pi, physicalAddress); 
}

#endif /* _ADDRESS_ */
