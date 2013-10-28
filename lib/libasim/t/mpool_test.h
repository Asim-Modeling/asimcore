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
 * Asim smart pointer and memory pool test.
 *
 * Intel, carl.j.beckmann@intel.com, January 2010
 */

#ifndef __MPOOL_TEST_H__
#define __MPOOL_TEST_H__

#include <vector>
#include <cxxtest/FTestSuite.h>

#define MAX_PTHREADS 2

#include "asim/syntax.h"
#include "asim/module.h"
#include "asim/smp.h"
#include "asim/mm.h"
#include "asim/mmptr.h"

using namespace std;

//
// some classes used in testing
//
template <typename T, size_t S>
struct A_BUFFER {
    T values[S];
};

static const size_t SMALL_ASIZE = 300;
class A_SMALL_OBJECT_CLASS : public ASIM_MM_CLASS<A_SMALL_OBJECT_CLASS>,
                             public A_BUFFER<char, SMALL_ASIZE>
{
};
typedef class mmptr<A_SMALL_OBJECT_CLASS> A_SMALL_OBJECT;

static const size_t SINGLE_ASIZE = 65536;
class A_SINGLE_OBJECT_CLASS : public ASIM_MM_CLASS<A_SINGLE_OBJECT_CLASS>,
                              public A_BUFFER<INT32, SINGLE_ASIZE>
{
};
typedef class mmptr<A_SINGLE_OBJECT_CLASS> A_SINGLE_OBJECT;

static const size_t LARGE_ASIZE = 999;
class A_LARGE_OBJECT_CLASS : public ASIM_MM_CLASS<A_LARGE_OBJECT_CLASS>,
                             public A_BUFFER<double, LARGE_ASIZE>
{
};
typedef class mmptr<A_LARGE_OBJECT_CLASS> A_LARGE_OBJECT;

//
// initial static sizing of the memory pools
//
static const UINT32 SMALL_OBJECTS_MAX = 100;
static const UINT32 SINGLE_OBJECT_MAX =   1;
static const UINT32 LARGE_OBJECTS_MAX =  50;
ASIM_MM_DEFINE(A_SMALL_OBJECT_CLASS,  SMALL_OBJECTS_MAX);
ASIM_MM_DEFINE(A_SINGLE_OBJECT_CLASS, SINGLE_OBJECT_MAX);
ASIM_MM_DEFINE(A_LARGE_OBJECT_CLASS,  LARGE_OBJECTS_MAX);

//
// the actual test suite.
//
class PortTestSuite : public CxxTest::TestSuite
{
public:

    // test that we can allocate the max number of objects specified statically
    void testAllocStaticMax() {
        std::vector<A_SMALL_OBJECT> pointers(SMALL_ASIZE);
        for (UINT32 i=0; i<SMALL_OBJECTS_MAX; i++)
            pointers[i] = new A_SMALL_OBJECT_CLASS;
    }
    
    // test that we can deallocate by smart pointers going out of scope
    void testDeallocScope() {
        {
            // allocate some storage in this block:
            std::vector<A_SMALL_OBJECT> a(SMALL_ASIZE);
            for (UINT32 i=0; i<SMALL_OBJECTS_MAX; i++)
                a[i] = new A_SMALL_OBJECT_CLASS;
            // end of block
        }
        {
            // allocate again in another scope:
            std::vector<A_SMALL_OBJECT> b(SMALL_ASIZE);
            for (UINT32 i=0; i<SMALL_OBJECTS_MAX; i++)
                b[i] = new A_SMALL_OBJECT_CLASS;
        }
    }
    
    // test that we can deallocate by assigning NULL to smart pointers
    void testDeallocNullAssign() {
        std::vector<A_SMALL_OBJECT> c(SMALL_ASIZE);
        // allocate
        for (UINT32 i=0; i<SMALL_OBJECTS_MAX; i++)
            c[i] = new A_SMALL_OBJECT_CLASS;
        // deallocate by assigning null to pointers
        for (UINT32 i=0; i<SMALL_OBJECTS_MAX; i++)
            c[i] = NULL;
        // allocate again
        std::vector<A_SMALL_OBJECT> d(SMALL_ASIZE);
        for (UINT32 i=0; i<SMALL_OBJECTS_MAX; i++)
            d[i] = new A_SMALL_OBJECT_CLASS;
    }
    
    // test that we can dynamically increase the max number of objects
    void testDynamicMax() {
        const UINT32 BIGGER_MAX = 777;
        std::vector<A_SMALL_OBJECT> lots(BIGGER_MAX);
        // allocating this many should be ok regardless:
        UINT32 i;
        for (i=0; i<SMALL_OBJECTS_MAX; i++)
            lots[i] = new A_SMALL_OBJECT_CLASS;
        // now increase the pool size:
        A_SMALL_OBJECT_CLASS::SetMaxObjs(BIGGER_MAX);
        // allocate some more:
        for (   ; i<BIGGER_MAX; i++)
            lots[i] = new A_SMALL_OBJECT_CLASS;
        // sanity check that we really increased the size over the static max:
        TS_ASSERT_LESS_THAN(SMALL_OBJECTS_MAX, i);
    }
    
    // test that storage is zeroed out on allocation
    // IS THIS REALLY A REQUIREMENT??
    void testZeroStorage() {
        A_SINGLE_OBJECT p = new A_SINGLE_OBJECT_CLASS;
        // put some nonzero data into the array:
        for (size_t i=0; i<SINGLE_ASIZE; i++)
            p->values[i] = i+1;
        // free the object, and allocate a new one
        p = NULL;
        p = new A_SINGLE_OBJECT_CLASS;
        // make sure it contains all zeroes:
        for (size_t i=0; i<SINGLE_ASIZE; i++)
            TS_ASSERT_EQUALS(p->values[i], 0);
    }
    
    // test the ability to change the max allocation size to something large
    // while keeping the initial preallocation size smaller.
    void testMaxAllocSizeChangeHack() {
        const UINT32 ABSOLUTE_MAX = 10000000; // 80GB if we allocated it all
        const UINT32 ACTUAL_ALLOC =    10000; // only 80 MB actually needed
        const UINT32 INITIAL_MAX  =     1000; // only 8 MB initial allocation
        std::vector<A_LARGE_OBJECT> large_array(ACTUAL_ALLOC);
        // set the initial allocation size
        A_LARGE_OBJECT_CLASS::SetMaxObjs(INITIAL_MAX);
        // force preallocation by newing a dummy element
        A_LARGE_OBJECT dummy = new A_LARGE_OBJECT_CLASS;
        // set the final max allocation size
        A_LARGE_OBJECT_CLASS::SetMaxObjs(ABSOLUTE_MAX);
        UINT32 i;
        for (i=0; i<ACTUAL_ALLOC; i++)
            large_array[i] = new A_LARGE_OBJECT_CLASS;
        TS_ASSERT_EQUALS(i, ACTUAL_ALLOC);
    }
    
    // test that we can set the chunk size for allocation
//    // FIX! NEED TO IMPLEMENT THIS FEATURE FIRST!
//    void testChunkSize() {
//        const UINT32 ABSOLUTE_MAX = 1000000;
//        const UINT32 ACTUAL_ALLOC =   10000;
//        const UINT32 CHUNK_SIZE   =    1000;
//        std::vector<A_LARGE_OBJECT> large_array(ACTUAL_ALLOC);
//        A_LARGE_OBJECT_CLASS::SetMaxObjs(ABSOLUTE_MAX);
//        A_LARGE_OBJECT_CLASS::SetAllocChunk(CHUNK_SIZE);
//        UINT32 i;
//        for (i=0; i<ACTUAL_ALLOC; i++)
//            large_array[i] = new A_LARGE_OBJECT_CLASS;
//        TS_ASSERT_EQUALS(i, ACTUAL_ALLOC);
//    }
};

#endif // __MPOOL_TEST_H__
