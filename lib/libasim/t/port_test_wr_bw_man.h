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

#ifndef __PORTTEST_H__
#define __PORTTEST_H__

#include <cxxtest/FTestSuite.h>

#include "asim/syntax.h"
#include "asim/module.h"
#include "asim/port.h"
#include "asim/clockserver.h"
#include "asim/rate_matcher.h"

using namespace std;

//
// some modules for use in testing.
// In Asim, we always assume that a port operates within the context of a module,
// even if it is not strictly necessary by the API.
//

// a module to serve as the top of the module hierarchy
class ASIM_SYSTEM_CLASS  : public ASIM_MODULE_CLASS {
    UINT64            last_cycle;            // last cycle executed
    ASIM_CLOCK_SERVER server;                // clock server
public:
    void Clock(UINT64 cycle)                 // clock routine records last cycle executed
    { last_cycle = cycle; }
    ASIM_SYSTEM_CLASS(ASIM_CLOCK_SERVER cs)  // constructor registers for a clock callback
    : ASIM_MODULE_CLASS(NULL, "system"), last_cycle(0), server(cs)
    { RegisterClock("CLOCK"); }
    void RunUntil(UINT64 end)                // run until the given clock cycle
    { while (last_cycle < end) server->Clock(); }
    void Step()                              // step a single clock cycle
    { RunUntil(last_cycle+1); }
    UINT64 GetCycle()                        // return the current clock cycle
    { return last_cycle; }
} *asimSystem = NULL;

// a module class with a single port of a templated class
template <class PORTCLASS>
class X_MODULE_CLASS : public ASIM_MODULE_CLASS {
public:
    PORTCLASS port;
    X_MODULE_CLASS(ASIM_MODULE parent, const char *iname)
      : ASIM_MODULE_CLASS(parent, iname) {}
};

//
// here's the actual test suite.
// In addition to always instantiating ports within the context of a module,
// we will always advance the clock using a clock server, rather than simply passing
// a new value for the clock cycle as a function argument to the port calls.
// The use of a clock server is the recommended Asim usage model, and is required
// for portability to Cameroon.
//
class PortTestSuite : public CxxTest::TestSuite
{
    ASIM_CLOCK_SERVER cs;  // the clock server
    static bool first;     // only perform certain clock server-related initializations the first time,
                           // since the Asim clock server is statically allocated and cannot be deleted.
                           // Instead, we instantiate it once, and reinitialize it for each test.
public:
    void setUp() {
        cs = ASIM_CLOCKABLE_CLASS::GetClockServer();
        if (first) {
            first = false;
            ASIM_SMP_CLASS::Init(1,1);
            list<float> freqs; freqs.push_back(1.0);
            cs->NewClockDomain("CLOCK", freqs);
        }
        asimSystem = new ASIM_SYSTEM_CLASS(cs);
        // do not initialize the clock server here, must be done after modules instantiated & connected
    }
    void tearDown() {
        cs->StopClockServer();
        cs->UnregisterAll();
        delete asimSystem;
    }
    
    // port with bandwidth > 1.
    // attempt to read more data than available.
    void testWriteBandwidthExceeded() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<int> > rm;
                        X_MODULE_CLASS<WritePort<int> > wm;
                        int                             data;
                        bool                            ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "q"),  true);
                        TS_ASSERT_EQUALS(rm.port.SetLatency(1),   true);
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "q"),  true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(2), true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 2: TS_ASSERT_EQUALS(wm.port.Write(0x1111, cycle), true);
                        TS_ASSERT_EQUALS(wm.port.Write(0x2222, cycle), true);
                        TS_ASSERT_EQUALS(wm.port.Write(0x3333, cycle), false); // exceeded BW!
                        break;
                case 3: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x1111);
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x2222);
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false); // nothing to read
                        TS_ASSERT_EQUALS(data, 0x2222); // previous value not overwritten
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(4);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
};

// first-time-through flag
bool PortTestSuite::first = true;

#endif // __PORTTEST_H__
