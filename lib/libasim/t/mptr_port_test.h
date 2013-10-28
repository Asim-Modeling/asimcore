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
 * Asim MM pointer port unit tests.
 *
 * Test writing mmptr's over a port, to see when ref count garbage collection actually happens.
 *
 * Intel, carl.j.beckmann@intel.com, August 2012
 */

#ifndef __MMPTR_PORTTEST_H__
#define __MMPTR_PORTTEST_H__

#include <cxxtest/FTestSuite.h>

#define MAX_PTHREADS 2

#include "asim/syntax.h"
#include "asim/module.h"
#include "asim/port.h"
#include "asim/clockserver.h"
#include "asim/smp.h"
#include "asim/mm.h"
#include "asim/mmptr.h"

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

// an object with smart pointers to it and a memory pool:
class AN_INT_CLASS : public ASIM_MM_CLASS<AN_INT_CLASS>
{
public:
    const UINT32 value;
    static UINT64 last_dtor;
    AN_INT_CLASS(UINT32 x) : value(x) {}
    ~AN_INT_CLASS() {
        // record the time at which destructor was called:
        last_dtor = asimSystem->GetCycle();
    }
};
typedef class mmptr<AN_INT_CLASS> AN_INT;
UINT64 AN_INT_CLASS::last_dtor = 0;
static const UINT32 AN_INT_POOL_SIZE = 10;
ASIM_MM_DEFINE(AN_INT_CLASS, AN_INT_POOL_SIZE);

// port latency
static const int PORT_LATENCY = 15;

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

    // a simple write-read test for a normal 1-cycle latency, unit bandwidth port
    void testWriteRead() {
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<AN_INT> > rm;    // reader module
                        X_MODULE_CLASS<WritePort<AN_INT> > wm;    // writer module
                        AN_INT                             data;  // location to receive read data
                        bool                               ok;    // flag is set on successful completion
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2"), true);
                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, PORT_LATENCY), true);
                        RegisterClock("CLOCK");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0:  TS_ASSERT_EQUALS(wm.port.Write(new AN_INT_CLASS(0xfeedbeef), cycle), true);
                         break;
                case PORT_LATENCY:
                         TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                         TS_ASSERT_EQUALS(data->value, (UINT32)0xfeedbeef);
                         // unreference the sent object, which should prompt garbage collection:
                         data = NULL;
                         ok = true;
                         break;
                default: // after we have sent and received the data,
                         // write NULLs into the port to flush it out:
                         if (cycle >= PORT_LATENCY + 2) {
                             TS_ASSERT_EQUALS(wm.port.Write(NULL, cycle), true);
                         }
                         break;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(2 * PORT_LATENCY + 5);
        TS_ASSERT_EQUALS(runner.ok, true);
        TS_ASSERT_EQUALS(AN_INT_CLASS::last_dtor, (UINT64)PORT_LATENCY);
    }

    // TODO: phase ports, config ports, peek ports
    
};

// first-time-through flag
bool PortTestSuite::first = true;

#endif // __MMPTR_PORTTEST_H__
