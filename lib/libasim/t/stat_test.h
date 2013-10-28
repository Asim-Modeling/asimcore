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
 * Asim stats unit tests.
 *
 * Intel, sailashri.parthasarathy@intel.com, January 2010
 */


#ifndef __STAT_TEST_H__
#define __STAT_TEST_H__

#include <cxxtest/FTestSuite.h>

#include "asim/syntax.h"
#include "asim/module.h"
#include "asim/clockserver.h"
#include "asim/registry.h"
#include "asim/state.h"

using namespace std;


// Some modules for use in testing.

// A module to serve as the top of the module hierarchy
class ASIM_SYSTEM_CLASS  : public ASIM_MODULE_CLASS {
public:
    ASIM_SYSTEM_CLASS() 
    : ASIM_MODULE_CLASS(NULL, "system") {};
} *asimSystem = NULL;


// A module class with stats of different data types
class X_MODULE_CLASS : public ASIM_MODULE_CLASS {

  public:
    X_MODULE_CLASS(ASIM_MODULE parent, const char *iname)
      : ASIM_MODULE_CLASS(parent, iname),
        uintStat (0),
        doubleStat (0.0),
        stringStat ("\0"),
        histoStat (1) // num_rows = 1
    { };
    
    UINT64 uintStat;
    double doubleStat;
    string stringStat;
    HISTOGRAM_TEMPLATE<true> histoStat;
};

class StatTestSuite : public CxxTest::TestSuite
{
  public:
    void setUp() {
        // Create a new ASIM_SYSTEM_CLASS that will serve as the parent
        // for all modules
        asimSystem = new ASIM_SYSTEM_CLASS();
    }
    void tearDown() {
        delete asimSystem;
    }
    

    // Test the RegisterStats function for uint
    void testRegisterStatUint() {
        X_MODULE_CLASS sm (asimSystem, "stat_module");
        ASIM_STATE as;

        as = sm.RegisterState (&sm.uintStat, "Uintstat", "Uint stat");

        TS_ASSERT_EQUALS (string(as->Name()), "Uintstat");
        TS_ASSERT_EQUALS (string(as->Description()), "Uint stat");
        TS_ASSERT_EQUALS (string(as->Path()), "/system/stat_module");
        TS_ASSERT_EQUALS (as->Size(), 1U);
    }


    // Test the RegisterStats function for a uint array
    void testRegisterStatUintArray() {
        X_MODULE_CLASS sm (asimSystem, "stat_module");
        UINT64 uStat[4];
        ASIM_STATE as;

        as = sm.RegisterState (uStat, 4, "UintArraystat", "Uint array stat");

        TS_ASSERT_EQUALS (string(as->Name()), "UintArraystat");
        TS_ASSERT_EQUALS (string(as->Description()), "Uint array stat");
        TS_ASSERT_EQUALS (string(as->Path()), "/system/stat_module");
        TS_ASSERT_EQUALS (as->Size(), 4U);
    }

    
    // Test the RegisterStats function for double
    void testRegisterStatDouble() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        ASIM_STATE as;

        as = sm.RegisterState (&sm.doubleStat, "Doublestat", "Double stat");

        // Check if Register State registered the stat correctly
        TS_ASSERT_EQUALS (string(as->Name()), "Doublestat");
        TS_ASSERT_EQUALS (string(as->Description()), "Double stat");
        TS_ASSERT_EQUALS (string(as->Path()), "/system/stat_module");
        TS_ASSERT_EQUALS (as->Size(), 1U);
    }


    // Test the RegisterStats function for string
    void testRegisterStatString() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        ASIM_STATE as;

        as = sm.RegisterState (&sm.stringStat, "Stringstat", "String stat");

        TS_ASSERT_EQUALS (string(as->Name()), "Stringstat");
        TS_ASSERT_EQUALS (string(as->Description()), "String stat");
        TS_ASSERT_EQUALS (string(as->Path()), "/system/stat_module");
        TS_ASSERT_EQUALS (as->Size(), 1U);
    }


    // Test the RegisterStats function for histograms
    void testRegisterStatHisto() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        ASIM_STATE as;

        as = sm.RegisterState (&sm.histoStat, "Histostat", "Histo stat");

        TS_ASSERT_EQUALS (string(as->Name()), "Histostat");
        TS_ASSERT_EQUALS (string(as->Description()), "Histo stat");
        TS_ASSERT_EQUALS (string(as->Path()), "/system/stat_module");
        TS_ASSERT_EQUALS (as->Size(), 1U);
    }


    // Test whether the values are assigned correctly for uint
    void testAssignValueUint() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        ASIM_STATE as;

        as = sm.RegisterState (&sm.uintStat, "Uintstat", "Uint stat");
        sm.uintStat = 1;
        
        TS_ASSERT_EQUALS (as->IntValue(), (UINT64) 1);
    } 


    // Test whether the values are assigned correctly for a uint array
    void testAssignValueUintArray() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        ASIM_STATE as;
        UINT64 uStat[4];

        as = sm.RegisterState (uStat, 4, "UintArraystat", "Uint array stat");

        uStat[0] = 1;
        uStat[1] = 2;
        uStat[2] = 3;
        uStat[3] = 4;
        
        // Returns the specified element
        TS_ASSERT_EQUALS (as->IntValue(0), (UINT64) 1);
        TS_ASSERT_EQUALS (as->IntValue(1), (UINT64) 2);
        TS_ASSERT_EQUALS (as->IntValue(2), (UINT64) 3);
        TS_ASSERT_EQUALS (as->IntValue(3), (UINT64) 4);
    }


    // Test the IntValue function for arrays - should return the sum
    void testIntValueArray() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        ASIM_STATE as;
        UINT64 uStat[4];

        as = sm.RegisterState (uStat, 4, "UintArraystat", "Uint array stat");

        uStat[0] = 1;
        uStat[1] = 2;
        uStat[2] = 3;
        uStat[3] = 4;
        
        // Returns the sum of all entries
        //        TS_ASSERT_EQUALS (as->IntValue(), 10);
    }


    // Test whether the values are assigned correctly for double
    void testAssignValueDouble() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        ASIM_STATE as;

        as = sm.RegisterState (&sm.doubleStat, "Doublestat", "Double stat");
        sm.doubleStat = 2.3;

        TS_ASSERT_EQUALS (as->FpValue(), 2.3);
    }
    

    // Test whether the values are assigned correctly for string
    void testAssignValueString() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        ASIM_STATE as;

        as = sm.RegisterState (&sm.stringStat, "Stringstat", "String stat");
        sm.stringStat = "testing";

        TS_ASSERT_EQUALS (as->StrValue(), "testing");
    }
    

    // Test whether the stats are incremented/decremented correctly
    void testIncDecStats() {
        X_MODULE_CLASS sm (asimSystem, "stat_module");
        ASIM_STATE as1, as2;

        as1 = sm.RegisterState (&sm.uintStat, "Uintstat", "Uint stat");
        as2 = sm.RegisterState (&sm.doubleStat, "Doublestat", "Double stat");
        
        sm.uintStat = 1;
        sm.doubleStat = 2.3;

        TS_ASSERT_EQUALS (as1->IntValue(), (UINT64) 1);
        TS_ASSERT_EQUALS (as2->FpValue(), 2.3);

        sm.uintStat++;
        sm.doubleStat++;

        // Check if the stats were incremented
        TS_ASSERT_EQUALS (as1->IntValue(), (UINT64) 2);
        TS_ASSERT_EQUALS (as2->FpValue(), 3.3);

        sm.uintStat--;
        sm.doubleStat--;

        // Check if the stats were decremented
        TS_ASSERT_EQUALS (as1->IntValue(), (UINT64) 1);
        TS_ASSERT_EQUALS (as2->FpValue(), 2.3);
    }


    // Test the AddEvent function for histograms
    void testAddEventHisto() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 

        HISTOGRAM_TEMPLATE<true> hStat (2); // num_rows = 2
        sm.RegisterState (&hStat, "Hstat", "Histo stat");

        TS_ASSERT_EQUALS (hStat.GetValue(0), 0U);
        TS_ASSERT_EQUALS (hStat.GetValue(1), 0U);

        hStat.AddEvent(0); // By default, assumes col=0, val=1
        hStat.AddEvent(1, 0, 3);

        TS_ASSERT_EQUALS (hStat.GetValue(0), 1U);
        TS_ASSERT_EQUALS (hStat.GetValue(1), 3U);
    }


    // The AddEventWideBins() function gets tested in the row_size/col_size tests
 
    // Test the row_flex_cap field for histograms
    void testRowFlexCapFalse() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        
        HISTOGRAM_TEMPLATE<true> hStat (2, 1, 1, false); // set row_flex_cap = false
        sm.RegisterState (&hStat, "Hstat", "Histo stat");

        hStat.AddEvent(0); 
        hStat.AddEvent(1);

        // Asim assertions currently dont throw anything. Hence commenting.
        //        TS_ASSERT_THROWS_ANYTHING (hStat.AddEvent(2));
    }


    // Test the row_flex_cap field for histograms
    void testRowFlexCapTrue() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        
        HISTOGRAM_TEMPLATE<true> hStat (2, 1, 1, true); // set row_flex_cap = true
        sm.RegisterState (&hStat, "Hstat", "Histo stat");

        hStat.AddEvent(0); 
        hStat.AddEvent(1);
        // Since the histogram has only 2 rows, any values in rows > 2 should be binned into row 2
        hStat.AddEvent(2);

        TS_ASSERT_EQUALS (hStat.GetValue(0), 1U);
        TS_ASSERT_EQUALS (hStat.GetValue(1), 2U);
    }


    // Test the col_flex_cap field for histograms
    void testColFlexCapFalse() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        
        HISTOGRAM_TEMPLATE<true> hStat (1, 2, 1, false, 1, false); // set col_flex_cap = false
        sm.RegisterState (&hStat, "Hstat", "Histo stat");

        hStat.AddEvent(0, 0); 
        hStat.AddEvent(0, 1);

        // Asim assertions currently dont throw anything. Hence commenting.
        //        TS_ASSERT_EQUALS (hStat.AddEvent(0,3), true);
    }


    // Test the col_flex_cap field for histograms
    void testColFlexCapTrue() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        
        HISTOGRAM_TEMPLATE<true> hStat (1, 2, 1, false, 1, true); // set col_flex_cap = true
        sm.RegisterState (&hStat, "Hstat", "Histo stat");

        hStat.AddEvent(0, 0); 
        hStat.AddEvent(0, 1);
        // Since the histogram has only 2 cols, any values in cols > 2 should be binned into col 2
        hStat.AddEvent(0, 3);

        TS_ASSERT_EQUALS (hStat.GetValue(0, 0), 1U);
        TS_ASSERT_EQUALS (hStat.GetValue(0, 1), 2U);
    }

    
    // Test the row_size field for histograms
    void testRowSize() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        
        HISTOGRAM_TEMPLATE<true> hStat (2, 1, 2); // set row_size = 2
        sm.RegisterState (&hStat, "Hstat", "Histo stat");

        hStat.AddEventWideBins(0); 
        hStat.AddEventWideBins(1);

        TS_ASSERT_EQUALS (hStat.GetValue(0), 2U);
        TS_ASSERT_EQUALS (hStat.GetValue(1), 2U);
        TS_ASSERT_EQUALS (hStat.GetValue(2), 0U);
        TS_ASSERT_EQUALS (hStat.GetValue(3), 0U);
    }


    // Test the col_size field for histograms
    void testColSize() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        
        HISTOGRAM_TEMPLATE<true> hStat (1, 2, 1, false, 2); // set col_size = 2
        sm.RegisterState (&hStat, "Hstat", "Histo stat");

        hStat.AddEventWideBins(0, 0); 
        hStat.AddEventWideBins(0, 2);

        TS_ASSERT_EQUALS (hStat.GetValue(0, 0), 1U);
        TS_ASSERT_EQUALS (hStat.GetValue(0, 1), 1U);
        TS_ASSERT_EQUALS (hStat.GetValue(0, 2), 1U);
        TS_ASSERT_EQUALS (hStat.GetValue(0, 3), 1U);
    }


    // Test whether the ClearStats function works
    void testClearStats() {
        X_MODULE_CLASS sm (asimSystem, "stat_module"); 
        ASIM_STATE as1, as2, as3, as4;

        as1 = sm.RegisterState (&sm.uintStat, "Uintstat", "Uint stat");
        as2 = sm.RegisterState (&sm.doubleStat, "Doublestat", "Double stat");
        as3 = sm.RegisterState (&sm.stringStat, "Stringstat", "String stat");
        as4 = sm.RegisterState (&sm.histoStat, "Histostat", "Histo stat");

        sm.uintStat = 1;
        sm.doubleStat = 2.3;
        sm.stringStat = "testing";
        sm.histoStat.AddEvent(0);
        
        TS_ASSERT_EQUALS (as1->IntValue(), (UINT64) 1);
        TS_ASSERT_EQUALS (as2->FpValue(), 2.3);
        TS_ASSERT_EQUALS (as3->StrValue(), "testing");
        TS_ASSERT_EQUALS (sm.histoStat.GetValue(0), 1U);

        // Clear the stats and check if values match the values they were initialized with
        sm.ClearModuleStats();
        TS_ASSERT_EQUALS (as1->IntValue(), (UINT64) 0);
        TS_ASSERT_EQUALS (as2->FpValue(), 0);
        TS_ASSERT_EQUALS (as3->StrValue(), "\0");
        TS_ASSERT_EQUALS (sm.histoStat.GetValue(0), 0U);
    }
};

#endif // __STAT_TEST_H__
