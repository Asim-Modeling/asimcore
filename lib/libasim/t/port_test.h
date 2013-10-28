/*
 *Copyright (C) 2010 Intel Corporation
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
 *
 * Asim port unit tests.
 *
 * This set of unit tests is focused primarily on the read and write functionality
 * of Asim ports (not on their connection).  It tests several different port types,
 * including normal read/write ports, skip ports, stall ports, and rate matchers.
 *
 * Intel, carl.j.beckmann@intel.com, August-September 2009
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
            list<float> f2; f2.push_back(2.0);
            cs->NewClockDomain("CLOCK2", f2);
            list<float> f3; f3.push_back(3.0);
            cs->NewClockDomain("CLOCK3", f3);
        }
        asimSystem = new ASIM_SYSTEM_CLASS(cs);
        // do not initialize the clock server here, must be done after modules instantiated & connected
    }
    void tearDown() {
        cs->StopClockServer();
        cs->UnregisterAll();
        delete asimSystem;
    }

    // test whether the IsConnected() function works
    void testConnect() {
        X_MODULE_CLASS< ReadPort<int> > rm(asimSystem, "reader");
        X_MODULE_CLASS<WritePort<int> > wm(asimSystem, "writer");
        TS_ASSERT_EQUALS(rm.port.IsConnected(), false);
        TS_ASSERT_EQUALS(wm.port.IsConnected(), false);
        TS_ASSERT_EQUALS(rm.port.InitConfig(&rm, "p1", 1, 1), true);
        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p1", 1, 1), true);
        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        TS_ASSERT_EQUALS(rm.port.IsConnected(), true);
        TS_ASSERT_EQUALS(wm.port.IsConnected(), true);
    }

    // a simple write-read test for a normal 1-cycle latency, unit bandwidth port
    void testWriteRead() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<int> > rm;    // reader module
                        X_MODULE_CLASS<WritePort<int> > wm;    // writer module
                        int                             data;  // location to receive read data
                        bool                            ok;    // flag is set on successful completion
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2"), true);
                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, 1), true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(wm.port.Write(0xfeedbeef, cycle), true);
                        break;
                case 1: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, (int)0xfeedbeef);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(2);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // port with bandwidth > 1.
    // attempt to read more data than available.
    void testBandwidth() {
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
                    //  TS_ASSERT_EQUALS(wm.port.Write(0x3333, cycle), false); // exceeded BW!
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
    
    // port with latency > 1
    void testLatency() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<int> > rm;
                        X_MODULE_CLASS<WritePort<int> > wm;
                        int                             data;
                        bool                            ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "zq"), true);
                        TS_ASSERT_EQUALS(rm.port.SetLatency(3),   true);
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "zq"), true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(1), true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(wm.port.Write(0xa1111, cycle), true);
                        break;
                case 1: TS_ASSERT_EQUALS(wm.port.Write(0xb2222, cycle), true);
                        break;
                case 2: TS_ASSERT_EQUALS(wm.port.Write(0xc3333, cycle), true);
                        break;
                case 3: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xa1111);
                        break;
                case 4: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xb2222);
                        break;
                case 5: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xc3333);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(6);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // fanout port: basic operation
    void testFanout() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<int>   > r1, r2;
                        X_MODULE_CLASS<WritePort<int,2> > wm;
                        int                               data;
                        bool                              ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        r1(this, "rdr1"), r2(this, "rdr2"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "fp"), true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(2), true);
                        TS_ASSERT_EQUALS(r1.port.Init(&r1, "fp"), true);
                        TS_ASSERT_EQUALS(r1.port.SetLatency(1),   true);
                        TS_ASSERT_EQUALS(r2.port.Init(&r2, "fp"), true);
                        TS_ASSERT_EQUALS(r2.port.SetLatency(1),   true);
                        TS_ASSERT_EQUALS(r1.port.IsConnected(),   false);
                        TS_ASSERT_EQUALS(r2.port.IsConnected(),   false);
                        TS_ASSERT_EQUALS(wm.port.IsConnected(),   false);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
                        TS_ASSERT_EQUALS(r1.port.IsConnected(),   true);
                        TS_ASSERT_EQUALS(r2.port.IsConnected(),   true);
                        TS_ASSERT_EQUALS(wm.port.IsConnected(),   true);
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 1: // write two items to the port
                        TS_ASSERT_EQUALS(wm.port.Write(0x1010101, cycle), true);
                        TS_ASSERT_EQUALS(wm.port.Write(0x2020202, cycle), true);
                        break;
                case 2: // reader 1 reads the two items
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x1010101);
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x2020202);
                        // reader 2 reads the two items
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x1010101);
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x2020202);
                        // write another item
                        TS_ASSERT_EQUALS(wm.port.Write(0x3030303, cycle), true);
                        break;
                case 3: // readers 1 and 2 both should be able to read the item
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x3030303);
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x3030303);
                        // neither reader 1 or two should see a second item
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), false);
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(4);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // fanout port: different latencies on the two readers.
    void testFanoutDiff() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<int>   > r1, r2;
                        X_MODULE_CLASS<WritePort<int,2> > wm;
                        int                               data;
                        bool                              ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        r1(this, "rdr1"), r2(this, "rdr2"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "fp"), true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(2), true);
                        TS_ASSERT_EQUALS(r1.port.Init(&r1, "fp"), true);
                        TS_ASSERT_EQUALS(r1.port.SetLatency(1),   true);
                        TS_ASSERT_EQUALS(r2.port.Init(&r2, "fp"), true);
                        TS_ASSERT_EQUALS(r2.port.SetLatency(2),   true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 1: // write two items to the port
                        TS_ASSERT_EQUALS(wm.port.Write(0x1F1F1F1, cycle), true);
                        TS_ASSERT_EQUALS(wm.port.Write(0x2F2F2F2, cycle), true);
                        break;
                case 2: // reader 1 reads the two items
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x1F1F1F1);
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x2F2F2F2);
                        // reader 2 reads nothing this cycle
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), false);
                        // write another item
                        TS_ASSERT_EQUALS(wm.port.Write(0x3F3F3F3, cycle), true);
                        break;
                case 3: // reader 2 reads the first two items
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x1F1F1F1);
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x2F2F2F2);
                        // readers 1 reads the third item
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x3F3F3F3);
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), false);
                        break;
                case 4: // readers 2 reads the third item
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x3F3F3F3);
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(5);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // stall port: basic port operation
    void testStallPortBasic() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS<  ReadStallPort<int> > rm;
                        X_MODULE_CLASS< WriteStallPort<int> > wm;
                        int                                   data;
                        bool                                  ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "sp"), true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(1), true);
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "sp"), true);
                        TS_ASSERT_EQUALS(rm.port.SetLatency(1),   true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 1: TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_EQUALS(wm.port.Write(0xABCDE, cycle), true);
                        break;
                case 2: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xABCDE);
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_EQUALS(wm.port.Write(0xDEF00, cycle), true);
                        break;
                case 3: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xDEF00);
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_EQUALS(wm.port.Write(0x98765, cycle), true);
                        break;
                case 4: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x98765);
                        break;
                case 5: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(6);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // stall port: does writer detect stall signal from reader?
    void testStallPortDetect() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS<  ReadStallPort<int> > rm;
                        X_MODULE_CLASS< WriteStallPort<int> > wm;
                        int data;
                        bool                            ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "spd"),true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(1), true);
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "spd"),true);
                        TS_ASSERT_EQUALS(rm.port.SetLatency(1),   true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 1: TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        break;
                case 2: TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_THROWS_NOTHING(rm.port.SetStalled(true));
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), true);
                        break;
                case 3: TS_ASSERT_EQUALS(wm.port.IsStalled(), true);
                        break;
                case 4: TS_ASSERT_EQUALS(wm.port.IsStalled(), true);
                        TS_ASSERT_THROWS_NOTHING(rm.port.SetStalled(false));
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        break;
                case 5: TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(6);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // stall port: does writer see stalled data after unstalling many cycles later?
    void testStallPortStall() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS<  ReadStallPort<int> > rm;
                        X_MODULE_CLASS< WriteStallPort<int> > wm;
                        int data;
                        bool                            ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "sps"),true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(1), true);
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "sps"),true);
                        TS_ASSERT_EQUALS(rm.port.SetLatency(1),   true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 1: TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_EQUALS(wm.port.Write(0xABCDE11, cycle), true);
                        // stall for a single cycle
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_THROWS_NOTHING(rm.port.Stall(true));
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), true);
                        break;
                case 2: TS_ASSERT_EQUALS(wm.port.IsStalled(), true);
                        TS_ASSERT_THROWS_NOTHING(rm.port.Stall(false));
                        // end stall
                        break;
                case 3: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xABCDE11);
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_EQUALS(wm.port.Write(0xDEF0011, cycle), true);
                        break;
                case 4: // stall for a single cycle - AFTER the step this time
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_THROWS_NOTHING(rm.port.Stall(true));
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), true);
                        break;
                case 5: TS_ASSERT_EQUALS(wm.port.IsStalled(), true);
                        TS_ASSERT_THROWS_NOTHING(rm.port.Stall(false));
                        // end stall
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xDEF0011);
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_EQUALS(wm.port.Write(0x9876511, cycle), true);
                        break;
                case 6: // stall for a multiple cycles -- reader must call Stall() each cycle!
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        TS_ASSERT_THROWS_NOTHING(rm.port.Stall(true));
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), true);
                        break;
                case 7: TS_ASSERT_THROWS_NOTHING(rm.port.Stall(true));
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false); // data unavailable during stall
                        break;
                case 8: TS_ASSERT_THROWS_NOTHING(rm.port.Stall(true));
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        break;
                case 9: TS_ASSERT_THROWS_NOTHING(rm.port.Stall(true));
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        break;
                case 10:TS_ASSERT_THROWS_NOTHING(rm.port.Stall(true));
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        break;
                case 11:TS_ASSERT_EQUALS(wm.port.IsStalled(), true);
                        TS_ASSERT_THROWS_NOTHING(rm.port.Stall(false));
                        TS_ASSERT_EQUALS(wm.port.IsStalled(), false);
                        // end stall
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x9876511);
                        break;
                case 12:TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(13);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // stall port: what happens if writer ignores stall signal?
    
    // skid port: basic operation
    void testSkidPortBasic() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS<  ReadSkidPort<int> > rm;
                        X_MODULE_CLASS< WriteSkidPort<int> > wm;
                        int data;
                        bool                            ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "skp"),true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(1), true);
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "skp"),true);
                        TS_ASSERT_EQUALS(rm.port.SetLatency(1),   true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 3: TS_ASSERT_EQUALS(wm.port.Write(0xABC, cycle), true);
                        break;
                case 4: TS_ASSERT_EQUALS(rm.port.Read(data, cycle),   true);
                        TS_ASSERT_EQUALS(data, 0xABC);
                        TS_ASSERT_EQUALS(wm.port.Write(0xDEF, cycle), true);
                        break;
                case 5: TS_ASSERT_EQUALS(rm.port.Read(data, cycle),   true);
                        TS_ASSERT_EQUALS(data, 0xDEF);
                        TS_ASSERT_EQUALS(wm.port.Write(0x987, cycle), true);
                        break;
                case 6: TS_ASSERT_EQUALS(rm.port.Read(data, cycle),   true);
                        TS_ASSERT_EQUALS(data, 0x987);
                        break;
                case 7: TS_ASSERT_EQUALS(rm.port.Read(data, cycle),   false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(8);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // skid port: lazy read
    void testSkidPortLazy() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS<  ReadSkidPort<int> > rm;
                        X_MODULE_CLASS< WriteSkidPort<int> > wm;
                        int data;
                        bool                            ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "skp"), true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(1),  true);
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "skp"), true);
                        TS_ASSERT_EQUALS(rm.port.SetLatency(1),    true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 3: TS_ASSERT_EQUALS(wm.port.Write(0x1ABC, cycle), true);
                        break;
                case 5: // extra clock!
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle),    true);
                        TS_ASSERT_EQUALS(data, 0x1ABC);
                        TS_ASSERT_EQUALS(wm.port.Write(0x1DEF, cycle), true);
                        break;
                case 8: // two extra clocks!
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle),    true);
                        TS_ASSERT_EQUALS(data, 0x1DEF);
                        TS_ASSERT_EQUALS(wm.port.Write(0x1987, cycle), true);
                        break; // no extra clocks.
                case 9: TS_ASSERT_EQUALS(rm.port.Read(data, cycle),    true);
                        TS_ASSERT_EQUALS(data, 0x1987);
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle),    false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(10);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // skid port: high bandwidth write, lazy reads
    void testSkidPortHiBWwrite() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS<  ReadSkidPort<int> > rm;
                        X_MODULE_CLASS< WriteSkidPort<int> > wm;
                        int data;
                        bool                            ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "skp"), true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(3),  true);
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "skp"), true);
                        TS_ASSERT_EQUALS(rm.port.SetLatency(2),    true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 3: TS_ASSERT_EQUALS(wm.port.Write(0x2ABC, cycle), true);
                        TS_ASSERT_EQUALS(wm.port.Write(0x2DEF, cycle), true);
                        TS_ASSERT_EQUALS(wm.port.Write(0x2987, cycle), true);
                        break;
                case 5: // extra clock
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle),    true);
                        TS_ASSERT_EQUALS(data, 0x2ABC);
                        break;
                case 6: TS_ASSERT_EQUALS(rm.port.Read(data, cycle),    true);
                        TS_ASSERT_EQUALS(data, 0x2DEF);
                        break;
                case 7: TS_ASSERT_EQUALS(rm.port.Read(data, cycle),    true);
                        TS_ASSERT_EQUALS(data, 0x2987);
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle),    false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(8);
        TS_ASSERT_EQUALS(runner.ok, true);
    }
    
    // skid port: sequential writes, high bandwidth reads
    // FIX LIBASIM!  This test currently fails with an assert on port.h:1198
//  void testSkidPortHiBWread() {
//                        X_MODULE_CLASS<  ReadSkidPort<int> > rm("reader");
//                        X_MODULE_CLASS< WriteSkidPort<int> > wm("writer");
//                        int data;
//                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "skp"), true);
//                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(3),  true);
//                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "skp"), true);
//                        TS_ASSERT_EQUALS(rm.port.SetLatency(2),    true);
//                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
//                          TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
//                        asimSystem->Step();
//                        asimSystem->Step();
//                        TS_ASSERT_EQUALS(wm.port.Write(0x2ABC, asimSystem->GetCycle()), true);
//                        asimSystem->Step();
//                        TS_ASSERT_EQUALS(wm.port.Write(0x2DEF, asimSystem->GetCycle()), true);
//                        asimSystem->Step();
//                        TS_ASSERT_EQUALS(wm.port.Write(0x2987, asimSystem->GetCycle()), true);
//                        asimSystem->Step();
//                        asimSystem->Step();
//                        TS_ASSERT_EQUALS(rm.port.Read(data, asimSystem->GetCycle()),    true);
//                        TS_ASSERT_EQUALS(data, 0x2ABC);
//                        TS_ASSERT_EQUALS(rm.port.Read(data, asimSystem->GetCycle()),    true);
//                        TS_ASSERT_EQUALS(data, 0x2DEF);
//                        TS_ASSERT_EQUALS(rm.port.Read(data, asimSystem->GetCycle()),    true);
//                        TS_ASSERT_EQUALS(data, 0x2987);
//                        TS_ASSERT_EQUALS(rm.port.Read(data, asimSystem->GetCycle()),    false);
//  }
    
    // rate matcher: 2:1 ratio: basic operation
    void testRateMatcher2to1() {
        class RMwriterClass : public ASIM_MODULE_CLASS {
          public:
            WriteRateMatcher<int> port;
            bool success;
            RMwriterClass()
              : ASIM_MODULE_CLASS(asimSystem, "writer"), port(this), success(false)
            {
                TS_ASSERT_THROWS_NOTHING(RegisterClock("CLOCK2"));
                TS_ASSERT_EQUALS(port.Init("rmp"),     true);
                TS_ASSERT_EQUALS(port.SetBandwidth(1), true);
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(port.Write(0x1111, cycle), true); break;
                case 1:                                                    break;
                case 2: TS_ASSERT_EQUALS(port.Write(0x2020, cycle), true); break;
                case 3:                                                    break;
                case 4: TS_ASSERT_EQUALS(port.Write(0x3330, cycle), true); break;
                case 5: success = true;
                }
            }
        } writer;
        class RMreaderClass : public ASIM_MODULE_CLASS {
          public:
            ReadRateMatcher<int> port;
            bool success;
            RMreaderClass()
              : ASIM_MODULE_CLASS(asimSystem, "reader"), port(this), success(false)
            {
                TS_ASSERT_THROWS_NOTHING(RegisterClock("CLOCK"));
                TS_ASSERT_EQUALS(port.Init("rmp"),   true);
                TS_ASSERT_EQUALS(port.SetLatency(1), true);
            }
            void Clock(UINT64 cycle) {
                int data = 0;
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 1: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x1111);                  break;
                case 2: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x2020);                  break;
                case 3: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0x3330);                  break;
                case 4: success = true;
                }
            }
        } reader;
        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        asimSystem->RunUntil(5);
        TS_ASSERT_EQUALS(writer.success, true);
        TS_ASSERT_EQUALS(reader.success, true);
    }
    
    // rate matcher: 3:2 ratio, and does writer detect CanWrite signal?
    //  we'll skew the reader a little to make it unambiguous:
    //   writer: -   a   b   -   c   d   -   e   f      ...
    //   reader:  -     -     a     b     c     d     e ...
    void testRateMatcher3to2() {
        class RMwriterClass : public ASIM_MODULE_CLASS {
          public:
            WriteRateMatcher<char> port;
            bool success;
            RMwriterClass()
              : ASIM_MODULE_CLASS(asimSystem, "writer"), port(this), success(false)
            {
                TS_ASSERT_THROWS_NOTHING(RegisterClock("CLOCK3"));
                TS_ASSERT_EQUALS(port.Init("rmp"),     true);
                TS_ASSERT_EQUALS(port.SetBandwidth(1), true);
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0:                                                 break;
                case 1: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('a', cycle), true); break;
                case 2: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('b', cycle), true); break;
                case 3: TS_ASSERT_EQUALS(port.CanWrite(),       false); break;
                case 4: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('c', cycle), true); break;
                case 5: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('d', cycle), true); break;
                case 6: TS_ASSERT_EQUALS(port.CanWrite(),       false); break;
                case 7: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('e', cycle), true); break;
                case 8: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('f', cycle), true); break;
                case 9 : success = true;
                }
            }
        } writer;
        class RMreaderClass : public ASIM_MODULE_CLASS {
          public:
            ReadRateMatcher<char> port;
            bool success;
            RMreaderClass()
              : ASIM_MODULE_CLASS(asimSystem, "reader"), port(this), success(false)
            {
                TS_ASSERT_THROWS_NOTHING(RegisterClock("CLOCK2", /*skew=*/1));
                TS_ASSERT_EQUALS(port.Init("rmp"),   true);
                TS_ASSERT_EQUALS(port.SetLatency(1), true);
            }
            void Clock(UINT64 cycle) {
                char data = '?';
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 1: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 2: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'a');                     break;
                case 3: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'b');                     break;
                case 4: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'c');                     break;
                case 5: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'd');                     break;
                case 6: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'e');                     break;
                case 7: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'f');                     break;
                case 8: success = true;
                }
            }
        } reader;
        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        asimSystem->RunUntil(5);
        TS_ASSERT_EQUALS(writer.success, true);
        TS_ASSERT_EQUALS(reader.success, true);
    }
    
    // rate matcher: 2:3 ratio, i.e. a slow writer
    //   writer: -     a     b     c     d     e     f  ...
    //   reader:  -   -   -   a   b   -   c   d   -   e ...
    void testRateMatcher2to3() {
        class RMwriterClass : public ASIM_MODULE_CLASS {
          public:
            WriteRateMatcher<char> port;
            bool success;
            RMwriterClass()
              : ASIM_MODULE_CLASS(asimSystem, "writer"), port(this), success(false)
            {
                TS_ASSERT_THROWS_NOTHING(RegisterClock("CLOCK2"));
                TS_ASSERT_EQUALS(port.Init("rmp"),     true);
                TS_ASSERT_EQUALS(port.SetBandwidth(1), true);
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0:                                                 break;
                case 1: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('A', cycle), true); break;
                case 2: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('B', cycle), true); break;
                case 3: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('C', cycle), true); break;
                case 4: TS_ASSERT_EQUALS(port.CanWrite(),        true);
                        TS_ASSERT_EQUALS(port.Write('D', cycle), true); break;
                case 5 : success = true;
                }
            }
        } writer;
        class RMreaderClass : public ASIM_MODULE_CLASS {
          public:
            ReadRateMatcher<char> port;
            bool success;
            RMreaderClass()
              : ASIM_MODULE_CLASS(asimSystem, "reader"), port(this), success(false)
            {
                TS_ASSERT_THROWS_NOTHING(RegisterClock("CLOCK3", /*skew=*/1));
                TS_ASSERT_EQUALS(port.Init("rmp"),   true);
                TS_ASSERT_EQUALS(port.SetLatency(1), true);
            }
            void Clock(UINT64 cycle) {
                char data = '?';
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 1: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 2: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 3: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'A');                     break;
                case 4: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'B');                     break;
                case 5: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 6: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'C');                     break;
                case 7: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 'D');                     break;
                case 8: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 9: success = true;
                }
            }
        } reader;
        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        asimSystem->RunUntil(4);
        TS_ASSERT_EQUALS(writer.success, true);
        TS_ASSERT_EQUALS(reader.success, true);
    }
      
    // multiple clock domains at once, using ordinary (non-rate-matcher) ports.
    // register for clock callbacks after initializing ports, typical Asim usage model.
    void testMultiClock() {
        class WriterClass : public ASIM_MODULE_CLASS {
          public:
            WritePort<int> port;
            bool success;
            WriterClass(const char *name, const char *port_name, const char *clock_domain)
              : ASIM_MODULE_CLASS(asimSystem, name), success(false)
            {
                TS_ASSERT_EQUALS(port.Init(this, port_name), true);
                TS_ASSERT_EQUALS(port.SetBandwidth(1), true);
                TS_ASSERT_THROWS_NOTHING(RegisterClock(clock_domain));
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(port.Write(0xA1111, cycle), true); break;
                case 1: TS_ASSERT_EQUALS(port.Write(0xB2020, cycle), true); break;
                case 2: TS_ASSERT_EQUALS(port.Write(0xC3330, cycle), true); break;
                case 3: success = true;
                }
            }
        } writer1("w1", "p", "CLOCK"), writer2("w2", "q", "CLOCK3");
        class ReaderClass : public ASIM_MODULE_CLASS {
          public:
            ReadPort<int> port;
            bool success;
            ReaderClass(const char *name, const char *port_name, const char *clock_domain)
              : ASIM_MODULE_CLASS(asimSystem, name), success(false)
            {
                TS_ASSERT_EQUALS(port.Init(this, port_name), true);
                TS_ASSERT_EQUALS(port.SetLatency(1), true);
                TS_ASSERT_THROWS_NOTHING(RegisterClock(clock_domain));
            }
            void Clock(UINT64 cycle) {
                int data = 0;
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 1: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xA1111);                 break;
                case 2: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xB2020);                 break;
                case 3: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xC3330);                 break;
                case 4: success = true;
                }
            }
        } reader1("r1", "p", "CLOCK"), reader2("r2", "q", "CLOCK3");
        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        asimSystem->RunUntil(5);
        TS_ASSERT_EQUALS(writer1.success, true);
        TS_ASSERT_EQUALS(reader1.success, true);
        TS_ASSERT_EQUALS(writer2.success, true);
        TS_ASSERT_EQUALS(reader2.success, true);
    }
      
    // clearing a port's contents
    void testClear() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<int> > rm;
                        X_MODULE_CLASS<WritePort<int> > wm;
                        int                             data;
                        bool                            ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "zq"), true);
                        TS_ASSERT_EQUALS(rm.port.SetLatency(3),   true);
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "zq"), true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(1), true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(wm.port.Write(0xaa1111, cycle), true);
                        break;
                case 1: TS_ASSERT_EQUALS(wm.port.Write(0xbb2222, cycle), true);
                        break;
                case 2: TS_ASSERT_EQUALS(wm.port.Write(0xcc3333, cycle), true);
                        break;
                case 3: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xaa1111);
                        break;
                case 4: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xbb2222);
                        break;
                case 5: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS(data, 0xcc3333);
                        break;
                case 6: TS_ASSERT_EQUALS(wm.port.Write(0xaaaa1111, cycle), true);
                        break;
                case 7: TS_ASSERT_EQUALS(wm.port.Write(0xbbbb2222, cycle), true);
                        break;
                case 8: TS_ASSERT_EQUALS(wm.port.Write(0xcccc3333, cycle), true);
                        break;
                        // clear it on the reader side:
                case 9: TS_ASSERT_THROWS_NOTHING(rm.port.Clear());
                        TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        break;
                case 10:TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        break;
                case 11:TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        break;
                case 12:TS_ASSERT_EQUALS(wm.port.Write(0x1111aaaa, cycle), true);
                        break;
                case 13:TS_ASSERT_EQUALS(wm.port.Write(0x2222bbbb, cycle), true);
                        break;
                case 14:TS_ASSERT_EQUALS(wm.port.Write(0x3333cccc, cycle), true);
                        // clear it on the writer side:
                        TS_ASSERT_THROWS_NOTHING(wm.port.Clear());
                        break;
                case 15:TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        break;
                case 16:TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        break;
                case 17:TS_ASSERT_EQUALS(rm.port.Read(data, cycle), false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(18);
        TS_ASSERT_EQUALS(runner.ok, true);
    }

    // TODO: phase ports, config ports, peek ports
    
};

// first-time-through flag
bool PortTestSuite::first = true;

#endif // __PORTTEST_H__
