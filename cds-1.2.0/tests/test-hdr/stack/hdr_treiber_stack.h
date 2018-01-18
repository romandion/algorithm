/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "cppunit/cppunit_proxy.h"
#include <cds/container/treiber_stack.h>

namespace stack {

    namespace cs = cds::container ;

    class TreiberTestHeader: public CppUnitMini::TestCase
    {
        template <class STACK>
        void test()
        {
            typedef typename STACK::value_type  value_type  ;
            STACK stack ;

            value_type v    ;

            CPPUNIT_ASSERT( stack.empty() ) ;

            CPPUNIT_ASSERT( stack.push(1))  ;
            CPPUNIT_ASSERT( !stack.empty() ) ;
            CPPUNIT_ASSERT( stack.push(2))  ;
            CPPUNIT_ASSERT( !stack.empty() ) ;
            CPPUNIT_ASSERT( stack.push(3))  ;
            CPPUNIT_ASSERT( !stack.empty() ) ;

            CPPUNIT_ASSERT( stack.pop(v) )  ;
            CPPUNIT_ASSERT( v == 3 )        ;
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.pop(v) )  ;
            CPPUNIT_ASSERT( v == 2 )        ;
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.pop(v) )  ;
            CPPUNIT_ASSERT( v == 1 )        ;
            CPPUNIT_ASSERT( stack.empty() ) ;
            v = 1000    ;
            CPPUNIT_ASSERT( !stack.pop(v) ) ;
            CPPUNIT_ASSERT( v == 1000 )     ;
            CPPUNIT_ASSERT( stack.empty() ) ;

            CPPUNIT_ASSERT( stack.push(10)) ;
            CPPUNIT_ASSERT( stack.push(20)) ;
            CPPUNIT_ASSERT( stack.push(30)) ;
            CPPUNIT_ASSERT( !stack.empty()) ;

            stack.clear()   ;
            CPPUNIT_ASSERT( stack.empty() ) ;

            STACK::gc::scan()   ;
        }

        void test_Treiber_HP();
        void test_Treiber_HRC();
        void test_Treiber_PTB();
        void test_Treiber_HP_yield();
        void test_Treiber_HRC_yield();
        void test_Treiber_PTB_yield();
        void test_Treiber_HP_pause_alloc();
        void test_Treiber_HRC_pause_alloc();
        void test_Treiber_PTB_pause_alloc();

        void test_Treiber_HP_relaxed();
        void test_Treiber_HRC_relaxed();
        void test_Treiber_PTB_relaxed();
        void test_Treiber_HP_yield_relaxed();
        void test_Treiber_HRC_yield_relaxed();
        void test_Treiber_PTB_yield_relaxed();
        void test_Treiber_HP_pause_alloc_relaxed();
        void test_Treiber_HRC_pause_alloc_relaxed();
        void test_Treiber_PTB_pause_alloc_relaxed();

        CPPUNIT_TEST_SUITE(TreiberTestHeader);
            CPPUNIT_TEST(test_Treiber_HP)
            CPPUNIT_TEST(test_Treiber_HP_relaxed)
            CPPUNIT_TEST(test_Treiber_HRC)
            CPPUNIT_TEST(test_Treiber_HRC_relaxed)
            CPPUNIT_TEST(test_Treiber_PTB)
            CPPUNIT_TEST(test_Treiber_PTB_relaxed)
            CPPUNIT_TEST(test_Treiber_HP_yield)
            CPPUNIT_TEST(test_Treiber_HP_yield_relaxed)
            CPPUNIT_TEST(test_Treiber_HRC_yield)
            CPPUNIT_TEST(test_Treiber_HRC_yield_relaxed)
            CPPUNIT_TEST(test_Treiber_PTB_yield)
            CPPUNIT_TEST(test_Treiber_PTB_yield_relaxed)
            CPPUNIT_TEST(test_Treiber_HP_pause_alloc)
            CPPUNIT_TEST(test_Treiber_HP_pause_alloc_relaxed)
            CPPUNIT_TEST(test_Treiber_HRC_pause_alloc)
            CPPUNIT_TEST(test_Treiber_HRC_pause_alloc_relaxed)
            CPPUNIT_TEST(test_Treiber_PTB_pause_alloc)
            CPPUNIT_TEST(test_Treiber_PTB_pause_alloc_relaxed)
        CPPUNIT_TEST_SUITE_END();
    };
}   // namespace stack

