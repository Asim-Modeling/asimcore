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

#ifndef __EVENTTEST_H__
#define __EVENTTEST_H__

#include <cxxtest/FTestSuite.h>

// HACK ALERT! normally these would be asim static parameters or compiler -D flags:
#define MAX_PTHREADS       1
#define ASIM_ENABLE_EVENTS 1

#include "asim/syntax.h"
#include "asim/module.h"
#include "asim/port.h"
#include "asim/clockserver.h"
#include "asim/rate_matcher.h"
#include "asim/mm.h"
#include "asim/item.h"
#include "asim/event.h"

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


// an Asim item to send over a port.
// Writing these items to ports should generate events.
class A_NORMAL_ITEM_CLASS : public ASIM_MM_CLASS<A_NORMAL_ITEM_CLASS>,
                            public ASIM_ITEM_CLASS {
private:
    int data;
public:
    A_NORMAL_ITEM_CLASS(int x = 0) : data(x) {}
    A_NORMAL_ITEM_CLASS &operator= (const A_NORMAL_ITEM_CLASS &x) { data = x.data; return *this; }
    operator int () { return data; }
};
typedef class mmptr<A_NORMAL_ITEM_CLASS> A_NORMAL_ITEM;

// an Asim silent item to send over a port.
// Writing these items to ports should NOT generate events.
class A_SILENT_ITEM_CLASS : public ASIM_MM_CLASS<A_SILENT_ITEM_CLASS>,
                            public ASIM_SILENT_ITEM_CLASS {
private:
    int data;
public:
    A_SILENT_ITEM_CLASS(int x = 0) : data(x) {}
    A_SILENT_ITEM_CLASS &operator= (const A_SILENT_ITEM_CLASS &x) { data = x.data; return *this; }
    operator int () { return data; }
};
typedef class mmptr<A_SILENT_ITEM_CLASS> A_SILENT_ITEM;

// a non-POD class, and pointers to it that we send over a port.
// Does not inherit from either ASIM_ITEM_CLASS nor ASIM_SILENT_ITEM_CLASS.
// No events should be generated, but writing pointers over ports should not crash.
class A_NONPOD_CLASS : public ASIM_MM_CLASS<A_NONPOD_CLASS> {
private:
    int data;
public:
    A_NONPOD_CLASS(int x = 0) : data(x) {}
    A_NONPOD_CLASS &operator= (const A_NONPOD_CLASS &x) { data = x.data; return *this; }
    ~A_NONPOD_CLASS ()              {}
    operator int    () { return  data; }
    virtual  int neg() { return -data; }
};
typedef class mmptr<A_NONPOD_CLASS> A_NONPOD_SMART_PTR;
typedef             A_NONPOD_CLASS *A_NONPOD_DUMB_PTR;


//
// declare memory pools for the different smart pointers
//
ASIM_MM_DEFINE(A_NORMAL_ITEM_CLASS, 100);
ASIM_MM_DEFINE(A_SILENT_ITEM_CLASS, 100);
ASIM_MM_DEFINE(A_NONPOD_CLASS,      100);


//
// here's the actual test suite.
// In addition to always instantiating ports within the context of a module,
// we will always advance the clock using a clock server, rather than simply passing
// a new value for the clock cycle as a function argument to the port calls.
// The use of a clock server is the recommended Asim usage model, and is required
// for portability to Cameroon.
//
class EventTestSuite : public CxxTest::TestSuite
{
    ASIM_CLOCK_SERVER cs;  // the clock server
    static bool first;     // only perform certain clock server-related initializations the first time,
                           // since the Asim clock server is statically allocated and cannot be deleted.
                           // Instead, we instantiate it once, and reinitialize it for each test.
public:
    // these are here so we can generate a separate events file for each separate test
    void StartEvents(const char *filename) {
        cs = ASIM_CLOCKABLE_CLASS::GetClockServer();
        if (first) {
            first = false;
            ASIM_SMP_CLASS::Init(1,1);
            list<float> freqs; freqs.push_back(1.0);
            cs->NewClockDomain("CLOCK", freqs);
        }
        ////////
        runWithEventsOn = true;
        if (ASIM_DRAL_EVENT_CLASS::event) delete ASIM_DRAL_EVENT_CLASS::event;
        ASIM_DRAL_EVENT_CLASS::event = new DRAL_SERVER_CLASS(filename,1024,true);
        ASIM_DRAL_EVENT_CLASS::event->NewNode("Fake_Node", 0);
        ////////
        asimSystem = new ASIM_SYSTEM_CLASS(cs);
        // do not initialize the clock server here, must be done after modules instantiated & connected
        ////////
        DRALEVENT(TurnOn());
        DRALEVENT(StartActivity(0));
    }
    void EndEvents() {
        DRALEVENT(TurnOff());
        runWithEventsOn = false;
        cs->StopClockServer();
        ////////
        cs->UnregisterAll();
        delete asimSystem;
    }
    
    // shorthand to instantiate a new clock domain in the default clock server
    static inline void NewClockDomain(const char *name, float f = 1.0) {
        list<float> freqs;
        freqs.push_back(f);
        ASIM_CLOCKABLE_CLASS::GetClockServer()->NewClockDomain(name, freqs);
    }

    // Writing an int over a port should not generate an event.
    void testIntPort() {
        StartEvents("testIntPort");
        NewClockDomain("clkA");
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<int> > rm;    // reader module
                        X_MODULE_CLASS<WritePort<int> > wm;    // writer module
                        int                             data;  // location to receive read data
                        bool                            ok;    // flag is set on successful completion
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2", rm.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, 1, wm.GetUniqueId()), true);
                        RegisterClock("clkA");
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
        EndEvents();
    }


    // Writing a silent item over a port should not generate an event.
    void testSilentItemPort() {
        StartEvents("testSilentItemPort");
        NewClockDomain("clkB");
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<A_SILENT_ITEM_CLASS> > rm;    // reader module
                        X_MODULE_CLASS<WritePort<A_SILENT_ITEM_CLASS> > wm;    // writer module
                        A_SILENT_ITEM_CLASS                             data;  // location to receive read data
                        bool                                            ok;    // flag is set on successful completion
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2", rm.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, 1, wm.GetUniqueId()), true);
                        RegisterClock("clkB");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(wm.port.Write(A_SILENT_ITEM_CLASS(0xf00dbeef), cycle), true);
                        break;
                case 1: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, (int)0xf00dbeef);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(2);
        TS_ASSERT_EQUALS(runner.ok, true);
        EndEvents();
    }


    // Writing a silent pointer over a port should not generate an event.
    void testSilentPointerPort() {
        StartEvents("testSilentPointerPort");
        NewClockDomain("clkC");
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<A_SILENT_ITEM> > rm;    // reader module
                        X_MODULE_CLASS<WritePort<A_SILENT_ITEM> > wm;    // writer module
                        A_SILENT_ITEM                             data;  // location to receive read data
                        bool                                    ok;    // flag is set on successful completion
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2", rm.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, 1, wm.GetUniqueId()), true);
                        RegisterClock("clkC");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(wm.port.Write(new A_SILENT_ITEM_CLASS(0xf00db00f), cycle), true);
                        break;
                case 1: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)(*data), (int)0xf00db00f);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(2);
        TS_ASSERT_EQUALS(runner.ok, true);
        EndEvents();
    }


    // Writing a normal item over a port SHOULD generate an event.
    void testNormalItemPort() {
        StartEvents("testNormalItemPort");
        NewClockDomain("clkD");
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<A_NORMAL_ITEM_CLASS> > rm;    // reader module
                        X_MODULE_CLASS<WritePort<A_NORMAL_ITEM_CLASS> > wm;    // writer module
                        A_NORMAL_ITEM_CLASS                             data;  // location to receive read data
                        bool                                            ok;    // flag is set on successful completion
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2", rm.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, 1, wm.GetUniqueId()), true);
                        RegisterClock("clkD");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(wm.port.Write(A_NORMAL_ITEM_CLASS(0xfeedb0ef), cycle), true);
                        break;
                case 1: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, (int)0xfeedb0ef);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(2);
        TS_ASSERT_EQUALS(runner.ok, true);
        EndEvents();
    }


    // Writing a normal pointer over a port SHOULD generate an event.
    void testNormalPointerPort() {
        StartEvents("testNormalPointerPort");
        NewClockDomain("clkE");
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<A_NORMAL_ITEM> > rm;    // reader module
                        X_MODULE_CLASS<WritePort<A_NORMAL_ITEM> > wm;    // writer module
                        A_NORMAL_ITEM                             data;  // location to receive read data
                        bool                                      ok;    // flag is set on successful completion
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2", rm.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, 1, wm.GetUniqueId()), true);
                        RegisterClock("clkE");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 2: TS_ASSERT_EQUALS(wm.port.Write(new A_NORMAL_ITEM_CLASS(0xf0edb0ef), cycle), true);
                        break;
                case 3: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)(*data), (int)0xf0edb0ef);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(4);
        TS_ASSERT_EQUALS(runner.ok, true);
        EndEvents();
    }


//    // Writing a non-POD item over a port MIGHT CRASH.
//    void testNonPODitemPort() {
//        StartEvents("testNonPODitemPort");
//        NewClockDomain("clkF");
//        class Runner : public ASIM_MODULE_CLASS {
//          public:
//                        X_MODULE_CLASS< ReadPort<A_NONPOD_CLASS> > rm;    // reader module
//                        X_MODULE_CLASS<WritePort<A_NONPOD_CLASS> > wm;    // writer module
//                        A_NONPOD_CLASS                             data;  // location to receive read data
//                        bool                                            ok;    // flag is set on successful completion
//            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
//                        rm(this, "reader"), wm(this, "writer"), ok(false)
//            {
//                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2", rm.GetUniqueId()), true);
//                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, 1, wm.GetUniqueId()), true);
//                        RegisterClock("clkF");
//                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
//                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
//            }
//            void Clock(UINT64 cycle) {
//                switch (cycle) {
//                case 0: TS_ASSERT_EQUALS(wm.port.Write(A_NONPOD_CLASS(0xfaadbeef), cycle), true);
//                        break;
//                case 1: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
//                        TS_ASSERT_EQUALS((int)data, (int)0xfaadbeef);
//                        ok = true;
//                }
//            }
//        } runner(cs);
//        asimSystem->RunUntil(2);
//        TS_ASSERT_EQUALS(runner.ok, true);
//        EndEvents();
//    }


    // Writing a non-POD pointer over a port SHOULD NOT GENERATE AN EVENT AND SHOULD NOT CRASH.
    void testNonPODpointerPort() {
        StartEvents("testNonPODpointerPort");
        NewClockDomain("clkG");
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<A_NONPOD_DUMB_PTR> > rm;    // reader module
                        X_MODULE_CLASS<WritePort<A_NONPOD_DUMB_PTR> > wm;    // writer module
                        A_NONPOD_DUMB_PTR                             data;  // location to receive read data
                        bool                                          ok;    // flag is set on successful completion
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2", rm.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, 1, wm.GetUniqueId()), true);
                        RegisterClock("clkG");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 2: TS_ASSERT_EQUALS(wm.port.Write(new A_NONPOD_CLASS(0xfaedbaef), cycle), true);
                        break;
                case 3: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)(*data), (int)0xfaedbaef);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(4);
        TS_ASSERT_EQUALS(runner.ok, true);
        EndEvents();
    }


    // Writing a non-POD pointer over a port SHOULD NOT GENERATE AN EVENT AND SHOULD NOT CRASH.
    void testNonPODsmartPointerPort() {
        StartEvents("testNonPODsmartPointerPort");
        NewClockDomain("clkH");
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<A_NONPOD_SMART_PTR> > rm;    // reader module
                        X_MODULE_CLASS<WritePort<A_NONPOD_SMART_PTR> > wm;    // writer module
                        A_NONPOD_SMART_PTR                             data;  // location to receive read data
                        bool                                           ok;    // flag is set on successful completion
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        rm(this, "reader"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(rm.port.Init(&rm, "p2", rm.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(wm.port.InitConfig(&wm, "p2", 1, 1, wm.GetUniqueId()), true);
                        RegisterClock("clkH");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 2: TS_ASSERT_EQUALS(wm.port.Write(new A_NONPOD_CLASS(0xfaedbaef), cycle), true);
                        break;
                case 3: TS_ASSERT_EQUALS(rm.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)(*data), (int)0xfaedbaef);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(4);
        TS_ASSERT_EQUALS(runner.ok, true);
        EndEvents();
    }


    // fanout port: we should get an event for each reader of the fanout
    void testFanout() {
        StartEvents("testFanout");
        NewClockDomain("clkI");
        class Runner : public ASIM_MODULE_CLASS {
          public:
                        X_MODULE_CLASS< ReadPort<A_NORMAL_ITEM_CLASS>   > r1, r2;
                        X_MODULE_CLASS<WritePort<A_NORMAL_ITEM_CLASS,2> > wm;
                        A_NORMAL_ITEM_CLASS                               data;
                        bool                                              ok;
            Runner(ASIM_CLOCK_SERVER cs) : ASIM_MODULE_CLASS(asimSystem, "runner"),
                        r1(this, "rdr1"), r2(this, "rdr2"), wm(this, "writer"), ok(false)
            {
                        TS_ASSERT_EQUALS(wm.port.Init(&wm, "fp", wm.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(r1.port.Init(&r1, "fp", r1.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(r2.port.Init(&r2, "fp", r2.GetUniqueId()), true);
                        TS_ASSERT_EQUALS(wm.port.SetBandwidth(2), true);
                        TS_ASSERT_EQUALS(r1.port.SetLatency(1),   true);
                        TS_ASSERT_EQUALS(r2.port.SetLatency(1),   true);
                        TS_ASSERT_EQUALS(r1.port.IsConnected(),   false);
                        TS_ASSERT_EQUALS(r2.port.IsConnected(),   false);
                        TS_ASSERT_EQUALS(wm.port.IsConnected(),   false);
                        RegisterClock("clkI");
                        TS_ASSERT_THROWS_NOTHING(BasePort::ConnectAll());
                        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
                        TS_ASSERT_EQUALS(r1.port.IsConnected(),   true);
                        TS_ASSERT_EQUALS(r2.port.IsConnected(),   true);
                        TS_ASSERT_EQUALS(wm.port.IsConnected(),   true);
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 3: // write two items to the port
                        TS_ASSERT_EQUALS(wm.port.Write(A_NORMAL_ITEM_CLASS(0x1010101), cycle), true);
                        TS_ASSERT_EQUALS(wm.port.Write(A_NORMAL_ITEM_CLASS(0x2020202), cycle), true);
                        break;
                case 4: // reader 1 reads the two items
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, 0x1010101);
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, 0x2020202);
                        // reader 2 reads the two items
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, 0x1010101);
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, 0x2020202);
                        // write another item
                        TS_ASSERT_EQUALS(wm.port.Write(A_NORMAL_ITEM_CLASS(0x3030303), cycle), true);
                        break;
                case 5: // readers 1 and 2 both should be able to read the item
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, 0x3030303);
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, 0x3030303);
                        // neither reader 1 or two should see a second item
                        TS_ASSERT_EQUALS(r1.port.Read(data, cycle), false);
                        TS_ASSERT_EQUALS(r2.port.Read(data, cycle), false);
                        ok = true;
                }
            }
        } runner(cs);
        asimSystem->RunUntil(6);
        TS_ASSERT_EQUALS(runner.ok, true);
        EndEvents();
    }
    
      
    // multiple clock domains at once, using ordinary (non-rate-matcher) ports.
    // register for clock callbacks after initializing ports, typical Asim usage model.
    void testMultiClock() {
        StartEvents("testMultiClock");
        NewClockDomain("CLOCK3", 3.0);
        class WriterClass : public ASIM_MODULE_CLASS {
          public:
            WritePort<A_NORMAL_ITEM_CLASS> port;
            bool success;
            WriterClass(const char *name, const char *port_name, const char *clock_domain)
              : ASIM_MODULE_CLASS(asimSystem, name), success(false)
            {
                TS_ASSERT_EQUALS(port.Init(this, port_name, this->GetUniqueId()), true);
                TS_ASSERT_EQUALS(port.SetBandwidth(1), true);
                TS_ASSERT_THROWS_NOTHING(RegisterClock(clock_domain));
            }
            void Clock(UINT64 cycle) {
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(port.Write(A_NORMAL_ITEM_CLASS(0xA1111), cycle), true); break;
                case 1: TS_ASSERT_EQUALS(port.Write(A_NORMAL_ITEM_CLASS(0xB2020), cycle), true); break;
                case 2: TS_ASSERT_EQUALS(port.Write(A_NORMAL_ITEM_CLASS(0xC3330), cycle), true); break;
                case 3: success = true;
                }
            }
        } writer1("w1", "p", "CLOCK"), writer2("w2", "q", "CLOCK3");
        class ReaderClass : public ASIM_MODULE_CLASS {
          public:
            ReadPort<A_NORMAL_ITEM_CLASS> port;
            bool success;
            ReaderClass(const char *name, const char *port_name, const char *clock_domain)
              : ASIM_MODULE_CLASS(asimSystem, name), success(false)
            {
                TS_ASSERT_EQUALS(port.Init(this, port_name, this->GetUniqueId()), true);
                TS_ASSERT_EQUALS(port.SetLatency(1), true);
                TS_ASSERT_THROWS_NOTHING(RegisterClock(clock_domain));
            }
            void Clock(UINT64 cycle) {
                A_NORMAL_ITEM_CLASS data(0);
                switch (cycle) {
                case 0: TS_ASSERT_EQUALS(port.Read(data, cycle), false); break;
                case 1: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, 0xA1111);            break;
                case 2: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, 0xB2020);            break;
                case 3: TS_ASSERT_EQUALS(port.Read(data, cycle), true);
                        TS_ASSERT_EQUALS((int)data, 0xC3330);            break;
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
        EndEvents();
    }
      
    
};

// first-time-through flag
bool EventTestSuite::first = true;

#endif // __EVENTTEST_H__
