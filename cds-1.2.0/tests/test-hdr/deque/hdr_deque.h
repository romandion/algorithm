/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "cppunit/cppunit_proxy.h"
#include "cppunit/test_beans.h"

namespace deque {

    class DequeHeaderTest: public CppUnitMini::TestCase
    {
    public:
        struct value_type {
            int     nVal    ;

            value_type()
            {}
            value_type( int i )
                : nVal(i)
                {}
        };

        struct assign_functor
        {
            template <typename T>
            void operator()( value_type& dest, T i ) const
            {
                dest.nVal = i ;
            }
        };

        struct pop_functor
        {
            template <typename T>
            void operator()( T& dest, value_type const& v )
            {
                dest = v.nVal   ;
            }
        };

        static void assign_func( value_type& dest, int i )
        {
            dest.nVal = i ;
        }
        static void pop_func( int& dest, value_type const& v )
        {
            dest = v.nVal ;
        }

        template <class Deque>
        void test()
        {
            test_beans::check_item_counter<typename Deque::item_counter> check_ic ;

            Deque q ;
            value_type v    ;
            int i ;

            CPPUNIT_ASSERT( q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 0 )) ;

            // push_right/pop_right
            CPPUNIT_ASSERT( q.push_back( value_type(5) )) ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 1 )) ;

            CPPUNIT_ASSERT( q.push_back( 10, assign_functor() )) ; // functor
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 2 )) ;

            CPPUNIT_ASSERT( q.push_back( 20, assign_func )) ;     // function
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 3 )) ;

            CPPUNIT_ASSERT( q.pop_back(v) )    ;
            CPPUNIT_ASSERT( v.nVal == 20 )  ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 2 )) ;

            CPPUNIT_ASSERT( q.pop_back( i, pop_functor()) )    ;
            CPPUNIT_ASSERT( i == 10 )  ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 1 )) ;

            CPPUNIT_ASSERT( q.pop_back(i, pop_func) )    ;
            CPPUNIT_ASSERT( i == 5 )  ;
            CPPUNIT_ASSERT( q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 0 )) ;

            v.nVal = -1 ;
            CPPUNIT_ASSERT( !q.pop_back(v) )    ;
            CPPUNIT_ASSERT( v.nVal == -1 )  ;
            CPPUNIT_ASSERT( q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 0 )) ;

            // push_right/pop_left
            CPPUNIT_ASSERT( q.push_back( value_type(5) )) ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 1 )) ;

            CPPUNIT_ASSERT( q.push_back( 10, assign_functor() )) ; // functor
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 2 )) ;

            CPPUNIT_ASSERT( q.push_back( 20, assign_func )) ;     // function
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 3 )) ;

            CPPUNIT_ASSERT( q.pop_front(v) )    ;
            CPPUNIT_ASSERT( v.nVal == 5 )  ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 2 )) ;

            CPPUNIT_ASSERT( q.pop_front(i, pop_functor() ) )    ;
            CPPUNIT_ASSERT( i == 10 )  ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 1 )) ;

            CPPUNIT_ASSERT( q.pop_front( i, pop_func ) )    ;
            CPPUNIT_ASSERT( i == 20 )  ;
            CPPUNIT_ASSERT( q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 0 )) ;

            v.nVal = -1 ;
            CPPUNIT_ASSERT( !q.pop_back(v) )    ;
            CPPUNIT_ASSERT( v.nVal == -1 )  ;
            CPPUNIT_ASSERT( q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 0 )) ;


            // push_left/pop_left
            CPPUNIT_ASSERT( q.push_front( value_type(5) )) ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 1 )) ;

            CPPUNIT_ASSERT( q.push_front( 10, assign_functor() )) ; // functor
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 2 )) ;

            CPPUNIT_ASSERT( q.push_front( 20, assign_func )) ;     // function
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 3 )) ;

            CPPUNIT_ASSERT( q.pop_front(v) )    ;
            CPPUNIT_ASSERT( v.nVal == 20 )  ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 2 )) ;

            CPPUNIT_ASSERT( q.pop_front(v) )    ;
            CPPUNIT_ASSERT( v.nVal == 10 )  ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 1 )) ;

            CPPUNIT_ASSERT( q.pop_front(v) )    ;
            CPPUNIT_ASSERT( v.nVal == 5 )  ;
            CPPUNIT_ASSERT( q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 0 )) ;

            // push_left/pop_right
            CPPUNIT_ASSERT( q.push_front( value_type(5) )) ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 1 )) ;

            CPPUNIT_ASSERT( q.push_front( 10, assign_functor() )) ; // functor
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 2 )) ;

            CPPUNIT_ASSERT( q.push_front( 20, assign_func )) ;     // function
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 3 )) ;

            CPPUNIT_ASSERT( q.pop_back(v) )    ;
            CPPUNIT_ASSERT( v.nVal == 5 )  ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 2 )) ;

            CPPUNIT_ASSERT( q.pop_back(v) )    ;
            CPPUNIT_ASSERT( v.nVal == 10 )  ;
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 1 )) ;

            CPPUNIT_ASSERT( q.pop_back(v) )    ;
            CPPUNIT_ASSERT( v.nVal == 20 )  ;
            CPPUNIT_ASSERT( q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 0 )) ;

            // clear test
            for ( int i = 0; i < 1000; i++ ) {
                CPPUNIT_ASSERT( q.push_back( value_type(i) ))   ;
                CPPUNIT_ASSERT( q.push_front( value_type(i * 1024) )) ;
            }
            CPPUNIT_ASSERT( !q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 2000 )) ;
            q.clear()   ;
            CPPUNIT_ASSERT( q.empty() ) ;
            CPPUNIT_ASSERT( check_ic( q.size(), 0 )) ;

        }

        void test_MichaelDeque_HP();
        void test_MichaelDeque_HP_ic();
        void test_MichaelDeque_HP_stat();
        void test_MichaelDeque_HP_noalign();

        void test_MichaelDeque_PTB();
        void test_MichaelDeque_PTB_ic();
        void test_MichaelDeque_PTB_stat();
        void test_MichaelDeque_PTB_noalign();

        CPPUNIT_TEST_SUITE(DequeHeaderTest)
            CPPUNIT_TEST( test_MichaelDeque_HP);
            CPPUNIT_TEST( test_MichaelDeque_HP_ic);
            CPPUNIT_TEST( test_MichaelDeque_HP_stat);
            CPPUNIT_TEST( test_MichaelDeque_HP_noalign);

            CPPUNIT_TEST( test_MichaelDeque_PTB);
            CPPUNIT_TEST( test_MichaelDeque_PTB_ic);
            CPPUNIT_TEST( test_MichaelDeque_PTB_stat);
            CPPUNIT_TEST( test_MichaelDeque_PTB_noalign);

        CPPUNIT_TEST_SUITE_END()

    };
}   // namespace deque
