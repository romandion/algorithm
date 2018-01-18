/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "cppunit/cppunit_proxy.h"
#include <cds/container/lazy_list_base.h>

namespace ordlist {
    namespace cc = cds::container       ;
    namespace co = cds::container::opt  ;

    class LazyKVListTestHeader: public CppUnitMini::TestCase
    {
    public:
        typedef int key_type    ;
        struct value_type {
            int m_val   ;

            value_type()
                : m_val(0)
            {}

            value_type( int n )
                : m_val( n )
            {}
        };

        template <typename T>
        struct lt
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1 < v2  ;
            }
        };

        template <typename T>
        struct cmp {
            int operator ()(const T& v1, const T& v2 ) const
            {
                if ( v1 < v2 )
                    return -1   ;
                return v1 > v2 ? 1 : 0  ;
            }
        };

        struct check_value {
            int     m_nExpected ;

            check_value( int nExpected )
                : m_nExpected( nExpected )
            {}

            template <typename T>
            void operator ()( T& pair )
            {
                CPPUNIT_ASSERT_CURRENT( pair.second.m_val == m_nExpected )  ;
            }
        };

        struct insert_functor {
            template <typename T>
            void operator()( T& pair )
            {
                pair.second.m_val = pair.first * 10 ;
            }
        };

        struct ensure_functor {
            template <typename T>
            void operator()( bool bNew, T& pair )
            {
                pair.second.m_val = pair.first * 50 ;
            }
        };

        struct erase_functor {
            int     nKey    ;
            int     nVal    ;

            template <typename T>
            void operator()( T& i )
            {
                nKey = i.first ;
                nVal = i.second.m_val;
            }
        };


    protected:
        template <class OrdList>
        void test()
        {
            typedef typename OrdList::value_type    value_type  ;

            {
                OrdList l   ;
                typename OrdList::iterator itTest ;
                typename OrdList::const_iterator citTest ;

                CPPUNIT_ASSERT( l.empty() ) ;

                // insert / find test
                CPPUNIT_ASSERT( !l.find( 100 ))   ;
                CPPUNIT_ASSERT( l.insert( 100 )) ;
                CPPUNIT_ASSERT( !l.empty() ) ;
                CPPUNIT_ASSERT( l.find( 100 ))   ;

                check_value chk(0)  ;
                CPPUNIT_ASSERT( l.find( 100, boost::ref( chk ) ))   ;

                CPPUNIT_ASSERT( !l.find( 50 ))   ;
                CPPUNIT_ASSERT( l.insert( 50, 500 )) ;
                CPPUNIT_ASSERT( l.find( 50 ))   ;
                CPPUNIT_ASSERT( !l.insert( 50, 5 )) ;
                chk.m_nExpected = 500   ;
                CPPUNIT_ASSERT( l.find( 50, boost::ref( chk ) ))   ;
                chk.m_nExpected = 0   ;
                CPPUNIT_ASSERT( l.find( 100, boost::ref( chk ) ))   ;
                CPPUNIT_ASSERT( !l.empty() ) ;

                CPPUNIT_ASSERT( !l.find( 150 ))   ;
                CPPUNIT_ASSERT( l.insert_key( 150, insert_functor() )) ;
                CPPUNIT_ASSERT( l.find( 150 ))   ;
                chk.m_nExpected = 1500   ;
                CPPUNIT_ASSERT( l.find( 150, boost::ref( chk ) ))   ;
                chk.m_nExpected = 0   ;
                CPPUNIT_ASSERT( l.find( 100, boost::ref( chk ) ))   ;
                chk.m_nExpected = 500   ;
                CPPUNIT_ASSERT( l.find( 50, boost::ref( chk ) ))   ;
                CPPUNIT_ASSERT( !l.empty() ) ;

                // erase test

                CPPUNIT_ASSERT( !l.erase( 500 ))    ;
                CPPUNIT_ASSERT( !l.empty() ) ;

                CPPUNIT_ASSERT( l.find( 50 ))   ;
                {
                    erase_functor ef    ;
                    l.erase( 50, boost::ref(ef))    ;
                    CPPUNIT_ASSERT( ef.nKey == 50 ) ;
                    CPPUNIT_ASSERT( ef.nVal == 500 ) ;
                }
                CPPUNIT_ASSERT( !l.find( 50 ))   ;

                // ensure test
                std::pair<bool, bool> bEnsureResult ;
                bEnsureResult = l.ensure( 100, ensure_functor() ) ;
                CPPUNIT_ASSERT( bEnsureResult.first )       ;
                CPPUNIT_ASSERT( !bEnsureResult.second )     ;
                chk.m_nExpected = 5000   ;
                CPPUNIT_ASSERT( l.find( 100, boost::ref( chk ) )) ;

                {
                    ensure_functor ef ;
                    bEnsureResult = l.ensure( 50, boost::ref( ef ))   ;
                }
                CPPUNIT_ASSERT( bEnsureResult.first )       ;
                CPPUNIT_ASSERT( bEnsureResult.second )     ;
                chk.m_nExpected = 2500   ;
                CPPUNIT_ASSERT( l.find( 50, boost::ref( chk ) )) ;

                // erase test
                CPPUNIT_ASSERT( !l.empty() ) ;
                CPPUNIT_ASSERT( l.erase( 100 ))    ;
                CPPUNIT_ASSERT( l.erase( 150 ))    ;
                CPPUNIT_ASSERT( l.erase( 50 ))      ;
                CPPUNIT_ASSERT( l.empty() ) ;

                // clear empty list
                l.clear()   ;
                CPPUNIT_ASSERT( l.empty() ) ;

                // Iterator test
                {
                    int nCount = 100   ;
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i, i * 2 ) )   ;

                    int i = 0   ;
                    for ( typename OrdList::iterator it = l.begin(), itEnd = l.end(); it != itEnd; ++it, ++i ) {
                        CPPUNIT_ASSERT( it.key() == i ) ;
                        CPPUNIT_ASSERT( it.val().m_val == i * 2 ) ;
                        it.val().m_val = i * 3    ;
                    }

                    // Check that we have visited all items
                    for ( int i = 0; i < nCount; ++i ) {
                        chk.m_nExpected = i * 3 ;
                        CPPUNIT_ASSERT( l.find( i, boost::ref(chk) ))   ;
                    }

                    l.clear()   ;
                    CPPUNIT_ASSERT( l.empty() ) ;

                    // Const iterator
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i, i * 7) )   ;

                    i = 0   ;
                    const OrdList& rl = l ;
                    for ( typename OrdList::const_iterator it = rl.begin(), itEnd = rl.end(); it != itEnd; ++it, ++i ) {
                        CPPUNIT_ASSERT( it.key() == i ) ;
                        CPPUNIT_ASSERT( it.val().m_val == i * 7 ) ;
                    }

                    // Check that we have visited all items
                    for ( int i = 0; i < nCount; ++i ) {
                        chk.m_nExpected = i * 7 ;
                        CPPUNIT_ASSERT( l.find( i, boost::ref(chk) ))   ;
                    }

                    l.clear()   ;
                    CPPUNIT_ASSERT( l.empty() ) ;
                }

            }
        }

        template <class OrdList>
        void nogc_test()
        {
            typedef typename OrdList::value_type    value_type  ;
            typedef typename OrdList::iterator      iterator    ;

            {
                OrdList l   ;
                iterator it ;

                CPPUNIT_ASSERT( l.empty() ) ;

                // insert / find test
                CPPUNIT_ASSERT( l.find( 100 ) == l.end() )      ;
                CPPUNIT_ASSERT( l.insert( 100 ) != l.end() )    ;
                CPPUNIT_ASSERT( !l.empty() )        ;
                it = l.find( 100 )  ;
                CPPUNIT_ASSERT( it != l.end() )     ;
                CPPUNIT_ASSERT( it.key() == 100 )   ;
                CPPUNIT_ASSERT( it.val().m_val == 0 )     ;

                CPPUNIT_ASSERT( l.find( 50 ) == l.end() )   ;
                CPPUNIT_ASSERT( l.insert( 50, 500 ) != l.end())    ;
                it = l.find( 50 )   ;
                CPPUNIT_ASSERT( it != l.end() )     ;
                CPPUNIT_ASSERT( it.key() == 50 )        ;
                CPPUNIT_ASSERT( it.val().m_val == 500 )       ;

                CPPUNIT_ASSERT( l.insert( 50, 5 ) == l.end() )     ;
                it = l.find( 50 )   ;
                CPPUNIT_ASSERT( it != l.end() )     ;
                CPPUNIT_ASSERT( it.key() == 50 )        ;
                CPPUNIT_ASSERT( it.val().m_val == 500 )       ;
                CPPUNIT_ASSERT( !l.empty() ) ;

                CPPUNIT_ASSERT( l.find( 150 ) == l.end() )   ;
                CPPUNIT_ASSERT( l.insert_key( 150, insert_functor() ) != l.end() ) ;
                it = l.find( 150 )   ;
                CPPUNIT_ASSERT( it != l.end() )     ;
                CPPUNIT_ASSERT( it.key() == 150 )       ;
                CPPUNIT_ASSERT( it.val().m_val == 1500 )      ;
                it = l.find( 100 )   ;
                CPPUNIT_ASSERT( it != l.end() )     ;
                CPPUNIT_ASSERT( it.key() == 100 )       ;
                CPPUNIT_ASSERT( it.val().m_val == 0 )         ;
                it = l.find( 50 )   ;
                CPPUNIT_ASSERT( it != l.end() )     ;
                CPPUNIT_ASSERT( it.key() == 50 )       ;
                CPPUNIT_ASSERT( it.val().m_val == 500 )      ;
                it.val().m_val = 25   ;
                it = l.find( 50 )   ;
                CPPUNIT_ASSERT( it != l.end() )     ;
                CPPUNIT_ASSERT( it.key() == 50 )       ;
                CPPUNIT_ASSERT( it.val().m_val == 25 )         ;
                CPPUNIT_ASSERT( !l.empty() ) ;

                // ensure existing item
                std::pair<iterator, bool> ensureResult ;
                ensureResult = l.ensure( 100 ) ;
                CPPUNIT_ASSERT( !ensureResult.second )      ;
                CPPUNIT_ASSERT( ensureResult.first.key() == 100 )   ;
                CPPUNIT_ASSERT( ensureResult.first.val().m_val == 0   )   ;
                ensureResult.first.val().m_val = 5    ;
                it = l.find( 100 )   ;
                CPPUNIT_ASSERT( it != l.end() )     ;
                CPPUNIT_ASSERT( it.key() == 100 )       ;
                CPPUNIT_ASSERT( it.val().m_val == 5 )         ;

                CPPUNIT_ASSERT( !l.empty() ) ;

                // ensure new item
                ensureResult = l.ensure( 1000 ) ;
                CPPUNIT_ASSERT( ensureResult.second )      ;
                CPPUNIT_ASSERT( ensureResult.first.key() == 1000 )  ;
                CPPUNIT_ASSERT( ensureResult.first.val().m_val == 0   )   ;
                ensureResult.first.val().m_val = 33    ;
                ensureResult = l.ensure( 1000 ) ;
                CPPUNIT_ASSERT( !ensureResult.second )      ;
                CPPUNIT_ASSERT( ensureResult.first.key() == 1000 )  ;
                CPPUNIT_ASSERT( ensureResult.first.val().m_val == 33   )  ;

                // clear test
                l.clear()   ;
                CPPUNIT_ASSERT( l.empty() ) ;

                // Iterator test
                {
                    int nCount = 100   ;
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i, i * 2 ) != l.end() )   ;

                    int i = 0   ;
                    for ( typename OrdList::iterator iter = l.begin(), itEnd = l.end(); iter != itEnd; ++iter, ++i ) {
                        CPPUNIT_ASSERT( iter.key() == i ) ;
                        CPPUNIT_ASSERT( iter->first == i ) ;
                        CPPUNIT_ASSERT( (*iter).first == i ) ;

                        CPPUNIT_ASSERT( iter.val().m_val == i * 2 ) ;
                        CPPUNIT_ASSERT( iter->second.m_val == i * 2 ) ;
                        CPPUNIT_ASSERT( (*iter).second.m_val == i * 2 ) ;

                        iter.val().m_val = i * 3    ;
                    }

                    // Check that we have visited all items
                    for ( int i = 0; i < nCount; ++i ) {
                        it = l.find( i )    ;
                        CPPUNIT_ASSERT( it != l.end() )     ;
                        CPPUNIT_ASSERT( it.key() == i )     ;
                        CPPUNIT_ASSERT( it.val().m_val == i * 3 ) ;
                    }

                    l.clear()   ;
                    CPPUNIT_ASSERT( l.empty() ) ;

                    // Const iterator
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i, i * 7) != l.end() )   ;

                    i = 0   ;
                    const OrdList& rl = l ;
                    for ( typename OrdList::const_iterator iter = rl.begin(), itEnd = rl.end(); iter != itEnd; ++iter, ++i ) {
                        CPPUNIT_ASSERT( iter.key() == i ) ;
                        CPPUNIT_ASSERT( iter->first == i ) ;
                        CPPUNIT_ASSERT( (*iter).first == i ) ;

                        CPPUNIT_ASSERT( iter.val().m_val == i * 7 ) ;
                        CPPUNIT_ASSERT( iter->second.m_val == i * 7 ) ;
                        CPPUNIT_ASSERT( (*iter).second.m_val == i * 7 ) ;
                        // it.val().m_val = i * 3    ; // error: const-iterator
                    }

                    l.clear()   ;
                    CPPUNIT_ASSERT( l.empty() ) ;
                }

            }
        }

        void HP_cmp()           ;
        void HP_less()          ;
        void HP_cmpmix()        ;
        void HP_ic()            ;

        void PTB_cmp()          ;
        void PTB_less()         ;
        void PTB_cmpmix()       ;
        void PTB_ic()           ;

        void HRC_cmp()          ;
        void HRC_less()         ;
        void HRC_cmpmix()       ;
        void HRC_ic()           ;

        void NOGC_cmp()         ;
        void NOGC_less()        ;
        void NOGC_cmpmix()      ;
        void NOGC_ic()          ;

        CPPUNIT_TEST_SUITE(LazyKVListTestHeader)
            CPPUNIT_TEST(HP_cmp)
            CPPUNIT_TEST(HP_less)
            CPPUNIT_TEST(HP_cmpmix)
            CPPUNIT_TEST(HP_ic)

            CPPUNIT_TEST(PTB_cmp)
            CPPUNIT_TEST(PTB_less)
            CPPUNIT_TEST(PTB_cmpmix)
            CPPUNIT_TEST(PTB_ic)

            CPPUNIT_TEST(HRC_cmp)
            CPPUNIT_TEST(HRC_less)
            CPPUNIT_TEST(HRC_cmpmix)
            CPPUNIT_TEST(HRC_ic)

            CPPUNIT_TEST(NOGC_cmp)
            CPPUNIT_TEST(NOGC_less)
            CPPUNIT_TEST(NOGC_cmpmix)
            CPPUNIT_TEST(NOGC_ic)
        CPPUNIT_TEST_SUITE_END()
    };

}   // namespace ordlist
