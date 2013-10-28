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
 * Asim clock server unit tests.
 *
 * Intel, carl.j.beckmann@intel.com, December 2009
 */

#ifndef __CLOCKSERVER_TEST_H__
#define __CLOCKSERVER_TEST_H__

#include <cxxtest/FTestSuite.h>

#include "asim/syntax.h"
#include "asim/module.h"
#include "asim/clockserver.h"

using namespace std;

//
// some modules for use in testing.
//

// a module class with a clock callback that does some checking
class CALLBACK_CHECKER_CLASS : public ASIM_MODULE_CLASS {
public:
    bool              first_call;            // flag is true only on the first callback
    UINT64            last_cycle;            // last cycle executed

    CALLBACK_CHECKER_CLASS(ASIM_MODULE parent, const char *iname, const char *clock_name)
      : ASIM_MODULE_CLASS(parent, iname),
        first_call(true),
        last_cycle(0)
    {
        RegisterClock(clock_name);
    }

    // this constructor registers a clock callback with skew
    CALLBACK_CHECKER_CLASS(ASIM_MODULE parent, const char *iname, const char *clock_name, int skew)
      : ASIM_MODULE_CLASS(parent, iname),
        first_call(true),
        last_cycle(0)
    {
        RegisterClock(clock_name, skew);
    }

    void Clock(UINT64 cycle)
    {
        if (first_call)
        {
            first_call = false;
        }
        else
        {
            // verify that we are getting a callback on each cycle, in order
            TS_ASSERT_EQUALS(cycle, last_cycle+1);
            last_cycle = cycle;
        }
    }
};

// similar to above, but with a different clock routine name
class ALTERNATE_CHECKER_CLASS : public ASIM_MODULE_CLASS {
public:
    bool   first_call;
    UINT64 last_cycle;

    // different clock routine name!
    void Invoke(UINT64 cycle)
    {
        if (first_call)
        {
            first_call = false;
        }
        else
        {
            TS_ASSERT_EQUALS(cycle, last_cycle+1);
            last_cycle = cycle;
        }
    }

    ALTERNATE_CHECKER_CLASS(ASIM_MODULE parent, const char *iname, const char *cname)
      : ASIM_MODULE_CLASS(parent, iname),
        first_call(true),
        last_cycle(0)
    {
        RegisterClock( cname, newCallback(this, &ALTERNATE_CHECKER_CLASS::Invoke) );
    }
};

// a module class with a clock callback that checks two other skew callback modules.
// On odd cycles, the two values should match.  On even cycles, the first value is bigger.
class SKEW_CHECKER_CLASS : public ASIM_MODULE_CLASS {
public:
    bool              first_call;            // flag is true only on the first callback
    UINT64&           value1;                // the value from the first thread to check
    UINT64&           value2;                // the value from the second thread to check
    bool              ok;                    // did we run more than once?

    SKEW_CHECKER_CLASS(ASIM_MODULE parent,
                       const char *iname,
                       const char *clock_name,
                       int         skew,
                       UINT64     &v1,
                       UINT64     &v2)
      : ASIM_MODULE_CLASS(parent, iname),
        first_call(true),
        value1(v1),
        value2(v2),
        ok(false)
    {
        RegisterClock(clock_name, skew);
    }

    void Clock(UINT64 cycle)
    {
        ok = !first_call;
        if (first_call)
        {
            first_call = false;
        }
        else if (cycle & 1) // odd cycles
        {
            TS_ASSERT_EQUALS(value1, value2);
        }
        else                // even cycles
        {
            TS_ASSERT_EQUALS(value1, value2+1);
        }
    }
};

//
// here's the actual test suite.
//
class ClockserverTestSuite : public CxxTest::TestSuite
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
            list<float> f5; f5.push_back(5.0);
            cs->NewClockDomain("CLOCK5", f5);
        }
        // do not initialize the clock server here, must be done after modules instantiated & connected
    }
    void tearDown() {
        cs->StopClockServer();
        cs->UnregisterAll();
    }

    // test basic clock operation: can we register and receive callbacks from a single clock:
    void testClockBasic() {
        CALLBACK_CHECKER_CLASS runner(NULL, "runner", "CLOCK");
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        while (runner.last_cycle < 10)
        {
            TS_ASSERT_THROWS_NOTHING(cs->Clock());
        }
        TS_ASSERT_EQUALS(runner.first_call, false);
        TS_ASSERT_EQUALS(runner.last_cycle, 10U);
    }

    // same as above, but use a different name for clock callback routine:
    void testClockAltCallback() {
        ALTERNATE_CHECKER_CLASS runner(NULL, "runner", "CLOCK");
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        while (runner.last_cycle < 10)
        {
            TS_ASSERT_THROWS_NOTHING(cs->Clock());
        }
        TS_ASSERT_EQUALS(runner.first_call, false);
        TS_ASSERT_EQUALS(runner.last_cycle, 10U);
    }

    // two different clock callbacks, 1:2 ratio
    void testTwoClocks() {
        CALLBACK_CHECKER_CLASS one(NULL, "one", "CLOCK");
        CALLBACK_CHECKER_CLASS two(NULL, "two", "CLOCK2");
        TS_ASSERT_THROWS_NOTHING(cs->SetReferenceClockDomain("CLOCK2"));
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        while (one.last_cycle < 10)
        {
            TS_ASSERT_THROWS_NOTHING(cs->Clock());
        }
        TS_ASSERT_EQUALS(one.first_call, false);
        TS_ASSERT_EQUALS(one.last_cycle, 10U);
        TS_ASSERT_EQUALS(two.first_call, false);
        TS_ASSERT_EQUALS(two.last_cycle, 20U);
    }

    // two different clock callbacks, 2:3 ratio
    void testTwoClocks2to3() {
        CALLBACK_CHECKER_CLASS one(NULL, "one", "CLOCK2");
        CALLBACK_CHECKER_CLASS two(NULL, "two", "CLOCK3");
        TS_ASSERT_THROWS_NOTHING(cs->SetReferenceClockDomain("CLOCK3"));
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        while (one.last_cycle < 10)
        {
            TS_ASSERT_THROWS_NOTHING(cs->Clock());
        }
        TS_ASSERT_EQUALS(one.first_call, false);
        TS_ASSERT_EQUALS(one.last_cycle, 10U);
        TS_ASSERT_EQUALS(two.first_call, false);
        TS_ASSERT_EQUALS(two.last_cycle, 15U);
    }

    // clock skew.  Two slow clocks at different skews.  A third checker thread between them
    void testClockSkew() {
        CALLBACK_CHECKER_CLASS one(NULL, "one", "CLOCK");
        CALLBACK_CHECKER_CLASS two(NULL, "two", "CLOCK", /*skew=*/50);
        SKEW_CHECKER_CLASS checker(NULL, "chk", "CLOCK2",/*skew=*/50, one.last_cycle, two.last_cycle);
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        while (two.last_cycle < 10)
        {
            TS_ASSERT_THROWS_NOTHING(cs->Clock());
        }
        TS_ASSERT_EQUALS(one.first_call, false);
        TS_ASSERT_EQUALS(one.last_cycle, 10U);
        TS_ASSERT_EQUALS(two.first_call, false);
        TS_ASSERT_EQUALS(two.last_cycle, 10U);
        TS_ASSERT_EQUALS(checker.first_call, false);
        TS_ASSERT_EQUALS(checker.ok, true);
    }

    // test base frequency and cycles.
    void testBaseFrequencyCycles() {
        // create a 3 GHz and a 5 GHz clock:
        CALLBACK_CHECKER_CLASS one(NULL, "one", "CLOCK3");
        CALLBACK_CHECKER_CLASS two(NULL, "two", "CLOCK5");
        TS_ASSERT_THROWS_NOTHING(cs->SetReferenceClockDomain("CLOCK5"));
        TS_ASSERT_THROWS_NOTHING(cs->InitClockServer());
        // base frequency should be LCM(2,3,5) = 30 GHz:
        TS_ASSERT_EQUALS(cs->getBaseFrequency(), UINT64(30000));
        // run it for 6000 cycles of the 3 MHz clock, accumulate base cycles:
        UINT64 base_cycles = 0;
        while (one.last_cycle < 6000)
        {
            base_cycles += cs->Clock();
        }
        TS_ASSERT_EQUALS(one.first_call, false);
        TS_ASSERT_EQUALS(one.last_cycle, 6000U);
        TS_ASSERT_EQUALS(two.first_call, false);
        TS_ASSERT_EQUALS(two.last_cycle, 10000U);
        // this should correspond to two thousand nanoseconds:
        TS_ASSERT_EQUALS(cs->getNanosecond(), UINT64(2000));
        // and sixty thousand base frequency cyles:
        TS_ASSERT_EQUALS(base_cycles, UINT64(60000));
    }
};

// first-time-through flag
bool ClockserverTestSuite::first = true;

#endif // __CLOCKSERVER_TEST_H__
