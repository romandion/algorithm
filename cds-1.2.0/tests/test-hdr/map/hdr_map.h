/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "cppunit/cppunit_proxy.h"
#include <cds/container/michael_map_base.h>

namespace map {
    namespace cc = cds::container   ;
    namespace co = cds::opt         ;

    // MichaelHashSet based on MichaelList
    class HashMapHdrTest: public CppUnitMini::TestCase
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

        typedef std::pair<key_type const, value_type> pair_type ;

        struct less
        {
            bool operator ()(int v1, int v2 ) const
            {
                return v1 < v2  ;
            }
        };

        struct cmp {
            int operator ()(int v1, int v2 ) const
            {
                if ( v1 < v2 )
                    return -1   ;
                return v1 > v2 ? 1 : 0  ;
            }
        };

        struct hash_int {
            size_t operator()( int i ) const
            {
                return co::v::hash<int>()( i ) ;
            }
        };

        struct simple_item_counter {
            size_t  m_nCount    ;

            simple_item_counter()
                : m_nCount(0)
            {}

            size_t operator ++()
            {
                return ++m_nCount ;
            }

            size_t operator --()
            {
                return --m_nCount   ;
            }

            void reset()
            {
                m_nCount = 0    ;
            }

            operator size_t() const
            {
                return m_nCount ;
            }
        };

        struct map_traits: public cc::michael_map::type_traits
        {
            typedef hash_int            hash            ;
            typedef simple_item_counter item_counter    ;
        };

        template <typename Map>
        struct insert_functor
        {
            typedef typename Map::pair_type pair_type   ;

            // insert ftor
            void operator()( pair_type& item )
            {
                item.second.m_val = item.first * 3   ;
            }

            // ensure ftor
            void operator()( bool bNew, pair_type& item )
            {
                if ( bNew )
                    item.second.m_val = item.first * 2        ;
                else
                    item.second.m_val = item.first * 5       ;
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

        struct extract_functor
        {
            int *   m_pVal  ;
            void operator()( pair_type const& val )
            {
                *m_pVal = val.second.m_val   ;
            }
        };


        template <class Map>
        void test_int()
        {
            Map m( 52, 4 )  ;

            std::pair<bool, bool> ensureResult ;

            // insert
            CPPUNIT_ASSERT( m.empty() )     ;
            CPPUNIT_ASSERT( m.size() == 0 ) ;
            CPPUNIT_ASSERT( !m.find(25) )   ;
            CPPUNIT_ASSERT( m.insert( 25 ) )    ;   // value = 0
            CPPUNIT_ASSERT( m.find(25) )   ;
            CPPUNIT_ASSERT( !m.empty() ) ;
            CPPUNIT_ASSERT( m.size() == 1 ) ;
            CPPUNIT_ASSERT( m.find(25) )   ;

            CPPUNIT_ASSERT( !m.insert( 25 ) )    ;
            CPPUNIT_ASSERT( !m.empty() ) ;
            CPPUNIT_ASSERT( m.size() == 1 ) ;

            CPPUNIT_ASSERT( !m.find(10) )   ;
            CPPUNIT_ASSERT( m.insert( 10, 10 ) )    ;
            CPPUNIT_ASSERT( !m.empty() ) ;
            CPPUNIT_ASSERT( m.size() == 2 ) ;
            CPPUNIT_ASSERT( m.find(10) )   ;

            CPPUNIT_ASSERT( !m.insert( 10, 20 ) )    ;
            CPPUNIT_ASSERT( !m.empty() ) ;
            CPPUNIT_ASSERT( m.size() == 2 ) ;

            CPPUNIT_ASSERT( !m.find(30) )   ;
            CPPUNIT_ASSERT( m.insert_key( 30, insert_functor<Map>() ) )    ; // value = 90
            CPPUNIT_ASSERT( !m.empty() ) ;
            CPPUNIT_ASSERT( m.size() == 3 ) ;
            CPPUNIT_ASSERT( m.find(30) )   ;

            CPPUNIT_ASSERT( !m.insert_key( 10, insert_functor<Map>() ) )    ;
            CPPUNIT_ASSERT( !m.insert_key( 25, insert_functor<Map>() ) )    ;
            CPPUNIT_ASSERT( !m.insert_key( 30, insert_functor<Map>() ) )    ;

            // ensure (new key)
            CPPUNIT_ASSERT( !m.find(27) )   ;
            ensureResult = m.ensure( 27, insert_functor<Map>() ) ;   // value = 54
            CPPUNIT_ASSERT( ensureResult.first )    ;
            CPPUNIT_ASSERT( ensureResult.second )    ;
            CPPUNIT_ASSERT( m.find(27) )   ;

            // find test
            check_value chk(10) ;
            CPPUNIT_ASSERT( m.find( 10, cds::ref(chk) ))   ;
            chk.m_nExpected = 0 ;
            CPPUNIT_ASSERT( m.find( 25, boost::ref(chk) ))   ;
            chk.m_nExpected = 90 ;
            CPPUNIT_ASSERT( m.find( 30, boost::ref(chk) ))   ;
            chk.m_nExpected = 54 ;
            CPPUNIT_ASSERT( m.find( 27, boost::ref(chk) ))   ;

            ensureResult = m.ensure( 10, insert_functor<Map>() ) ;   // value = 50
            CPPUNIT_ASSERT( ensureResult.first )    ;
            CPPUNIT_ASSERT( !ensureResult.second )    ;
            chk.m_nExpected = 50 ;
            CPPUNIT_ASSERT( m.find( 10, boost::ref(chk) ))   ;

            // erase test
            CPPUNIT_ASSERT( !m.find(100) )   ;
            CPPUNIT_ASSERT( !m.erase( 100 )) ;  // not found

            CPPUNIT_ASSERT( m.find(25) )    ;
            CPPUNIT_ASSERT( m.size() == 4 ) ;
            CPPUNIT_ASSERT( m.erase( 25 ))  ;
            CPPUNIT_ASSERT( !m.empty() ) ;
            CPPUNIT_ASSERT( m.size() == 3 ) ;
            CPPUNIT_ASSERT( !m.find(25) )   ;
            CPPUNIT_ASSERT( !m.erase( 25 )) ;

            //value_type val  ;

            int nVal    ;
            extract_functor ext ;
            ext.m_pVal = &nVal  ;
            CPPUNIT_ASSERT( m.erase( 30, boost::ref(ext)))    ;
            CPPUNIT_ASSERT( !m.empty() ) ;
            CPPUNIT_ASSERT( m.size() == 2 ) ;
            CPPUNIT_ASSERT( nVal == 90 )    ;
            nVal = -1   ;
            CPPUNIT_ASSERT( !m.erase( 30, boost::ref(ext)))    ;
            CPPUNIT_ASSERT( nVal == -1 )    ;

            m.clear()   ;
            CPPUNIT_ASSERT( m.empty() ) ;
            CPPUNIT_ASSERT( m.size() == 0 ) ;

            // iterator test
            test_iter<Map>() ;
        }


        template <class Map>
        void test_int_nogc()
        {
            typedef typename Map::iterator          iterator    ;
            typedef typename Map::const_iterator    const_iterator  ;

            {
                Map m( 52, 4 )  ;

                CPPUNIT_ASSERT( m.empty() ) ;
                CPPUNIT_ASSERT( m.size() == 0 ) ;

                CPPUNIT_ASSERT( m.find(10) == m.end() ) ;
                iterator it = m.insert( 10 )   ;
                CPPUNIT_ASSERT( it != m.end() )     ;
                CPPUNIT_ASSERT( !m.empty() )        ;
                CPPUNIT_ASSERT( m.size() == 1 )     ;
                CPPUNIT_ASSERT( m.find(10) == it )  ;
                CPPUNIT_ASSERT( it->first == 10 )    ;
                CPPUNIT_ASSERT( it->second.m_val == 0 )     ;

                CPPUNIT_ASSERT( m.find(100) == m.end() ) ;
                it = m.insert( 100, 200 )   ;
                CPPUNIT_ASSERT( it != m.end() )     ;
                CPPUNIT_ASSERT( !m.empty() )        ;
                CPPUNIT_ASSERT( m.size() == 2 )     ;
                CPPUNIT_ASSERT( m.find(100) == it ) ;
                CPPUNIT_ASSERT( it->first == 100 )   ;
                CPPUNIT_ASSERT( it->second.m_val == 200 )   ;

                CPPUNIT_ASSERT( m.find(55) == m.end() ) ;
                it = m.insert_key( 55, insert_functor<Map>() )   ;
                CPPUNIT_ASSERT( it != m.end() )     ;
                CPPUNIT_ASSERT( !m.empty() )        ;
                CPPUNIT_ASSERT( m.size() == 3 )     ;
                CPPUNIT_ASSERT( m.find(55) == it )  ;
                CPPUNIT_ASSERT( it->first == 55 )    ;
                CPPUNIT_ASSERT( it->second.m_val == 55 * 3 )    ;

                CPPUNIT_ASSERT( m.insert( 55 ) == m.end() ) ;
                CPPUNIT_ASSERT( m.insert( 55, 10 ) == m.end() ) ;
                CPPUNIT_ASSERT( m.insert_key( 55, insert_functor<Map>()) == m.end() )   ;

                CPPUNIT_ASSERT( m.find(10) != m.end() ) ;
                std::pair<iterator, bool> ensureResult = m.ensure( 10 )   ;
                CPPUNIT_ASSERT( ensureResult.first != m.end() )     ;
                CPPUNIT_ASSERT( !ensureResult.second  )     ;
                CPPUNIT_ASSERT( !m.empty() )        ;
                ensureResult.first->second.m_val = ensureResult.first->first * 5 ;
                CPPUNIT_ASSERT( m.size() == 3 )     ;
                CPPUNIT_ASSERT( m.find(10) == ensureResult.first ) ;
                it = m.find(10) ;
                CPPUNIT_ASSERT( it != m.end() ) ;
                CPPUNIT_ASSERT( it->second.m_val == 50 )    ;

                CPPUNIT_ASSERT( m.find(120) == m.end() ) ;
                ensureResult = m.ensure( 120 )   ;
                CPPUNIT_ASSERT( ensureResult.first != m.end() )     ;
                CPPUNIT_ASSERT( ensureResult.second  )     ;
                CPPUNIT_ASSERT( !m.empty() )        ;
                CPPUNIT_ASSERT( m.size() == 4 )     ;
                ensureResult.first->second.m_val = ensureResult.first->first * 5 ;
                CPPUNIT_ASSERT( m.find(120) == ensureResult.first ) ;
                it = m.find(120)    ;
                CPPUNIT_ASSERT( it != m.end() ) ;
                CPPUNIT_ASSERT( it->second.m_val == 120 * 5 )    ;
            }

            // iterator test

            {
                Map m( 52, 4 )  ;

                for ( int i = 0; i < 500; ++i ) {
                    CPPUNIT_ASSERT( m.insert( i, i * 2 ) != m.end() )   ;
                }
                CPPUNIT_ASSERT( m.size() == 500 ) ;

                for ( iterator it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( it->first * 2 == (*it).second.m_val )    ;
                    it->second = it->first ;
                }

                Map const& refMap = m   ;
                for ( const_iterator it = refMap.begin(), itEnd = refMap.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( it->first == it->second.m_val )    ;
                    CPPUNIT_ASSERT( (*it).first == (*it).second.m_val )    ;
                }
            }
        }

        template <class Map>
        void test_iter()
        {
            typedef typename Map::iterator          iterator ;
            typedef typename Map::const_iterator    const_iterator ;

            Map s( 100, 4 )     ;

            const int nMaxCount = 500   ;
            for ( int i = 0; i < nMaxCount; ++i ) {
                CPPUNIT_ASSERT( s.insert( i, i * 2 ))   ;
            }

            int nCount = 0  ;
            for ( iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first * 2 == it->second.m_val )    ;
                CPPUNIT_ASSERT( (*it).first * 2 == (*it).second.m_val )    ;
                it->second.m_val = it->first   ;
                ++nCount    ;
            }
            CPPUNIT_ASSERT( nCount == nMaxCount ) ;

            Map const& refSet = s;
            nCount = 0  ;
            for ( const_iterator it = refSet.begin(), itEnd = refSet.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first == it->second.m_val )    ;
                CPPUNIT_ASSERT( (*it).first == (*it).second.m_val )    ;
                ++nCount    ;
            }
            CPPUNIT_ASSERT( nCount == nMaxCount ) ;
        }


        void HP_cmp()          ;
        void HP_less()         ;
        void HP_cmpmix()       ;

        void PTB_cmp()          ;
        void PTB_less()         ;
        void PTB_cmpmix()       ;

        void HRC_cmp()          ;
        void HRC_less()         ;
        void HRC_cmpmix()       ;

        void nogc_cmp()          ;
        void nogc_less()         ;
        void nogc_cmpmix()       ;

        void Lazy_HP_cmp()          ;
        void Lazy_HP_less()         ;
        void Lazy_HP_cmpmix()       ;

        void Lazy_PTB_cmp()          ;
        void Lazy_PTB_less()         ;
        void Lazy_PTB_cmpmix()       ;

        void Lazy_HRC_cmp()          ;
        void Lazy_HRC_less()         ;
        void Lazy_HRC_cmpmix()       ;

        void Lazy_nogc_cmp()          ;
        void Lazy_nogc_less()         ;
        void Lazy_nogc_cmpmix()       ;

        void Split_HP_cmp()          ;
        void Split_HP_less()         ;
        void Split_HP_cmpmix()       ;

        void Split_PTB_cmp()          ;
        void Split_PTB_less()         ;
        void Split_PTB_cmpmix()       ;

        void Split_HRC_cmp()          ;
        void Split_HRC_less()         ;
        void Split_HRC_cmpmix()       ;

        void Split_nogc_cmp()          ;
        void Split_nogc_less()         ;
        void Split_nogc_cmpmix()       ;

        void Split_Lazy_HP_cmp()          ;
        void Split_Lazy_HP_less()         ;
        void Split_Lazy_HP_cmpmix()       ;

        void Split_Lazy_PTB_cmp()          ;
        void Split_Lazy_PTB_less()         ;
        void Split_Lazy_PTB_cmpmix()       ;

        void Split_Lazy_HRC_cmp()          ;
        void Split_Lazy_HRC_less()         ;
        void Split_Lazy_HRC_cmpmix()       ;

        void Split_Lazy_nogc_cmp()          ;
        void Split_Lazy_nogc_less()         ;
        void Split_Lazy_nogc_cmpmix()       ;

        CPPUNIT_TEST_SUITE(HashMapHdrTest)
            CPPUNIT_TEST(HP_cmp)
            CPPUNIT_TEST(HP_less)
            CPPUNIT_TEST(HP_cmpmix)

            CPPUNIT_TEST(PTB_cmp)
            CPPUNIT_TEST(PTB_less)
            CPPUNIT_TEST(PTB_cmpmix)

            CPPUNIT_TEST(HRC_cmp)
            CPPUNIT_TEST(HRC_less)
            CPPUNIT_TEST(HRC_cmpmix)

            CPPUNIT_TEST(nogc_cmp)
            CPPUNIT_TEST(nogc_less)
            CPPUNIT_TEST(nogc_cmpmix)

            CPPUNIT_TEST(Lazy_HP_cmp)
            CPPUNIT_TEST(Lazy_HP_less)
            CPPUNIT_TEST(Lazy_HP_cmpmix)

            CPPUNIT_TEST(Lazy_PTB_cmp)
            CPPUNIT_TEST(Lazy_PTB_less)
            CPPUNIT_TEST(Lazy_PTB_cmpmix)

            CPPUNIT_TEST(Lazy_HRC_cmp)
            CPPUNIT_TEST(Lazy_HRC_less)
            CPPUNIT_TEST(Lazy_HRC_cmpmix)

            CPPUNIT_TEST(Lazy_nogc_cmp)
            CPPUNIT_TEST(Lazy_nogc_less)
            CPPUNIT_TEST(Lazy_nogc_cmpmix)

            CPPUNIT_TEST(Split_HP_cmp)
            CPPUNIT_TEST(Split_HP_less)
            CPPUNIT_TEST(Split_HP_cmpmix)

            CPPUNIT_TEST(Split_PTB_cmp)
            CPPUNIT_TEST(Split_PTB_less)
            CPPUNIT_TEST(Split_PTB_cmpmix)

            CPPUNIT_TEST(Split_HRC_cmp)
            CPPUNIT_TEST(Split_HRC_less)
            CPPUNIT_TEST(Split_HRC_cmpmix)

            CPPUNIT_TEST(Split_nogc_cmp)
            CPPUNIT_TEST(Split_nogc_less)
            CPPUNIT_TEST(Split_nogc_cmpmix)

            CPPUNIT_TEST(Split_Lazy_HP_cmp)
            CPPUNIT_TEST(Split_Lazy_HP_less)
            CPPUNIT_TEST(Split_Lazy_HP_cmpmix)

            CPPUNIT_TEST(Split_Lazy_PTB_cmp)
            CPPUNIT_TEST(Split_Lazy_PTB_less)
            CPPUNIT_TEST(Split_Lazy_PTB_cmpmix)

            CPPUNIT_TEST(Split_Lazy_HRC_cmp)
            CPPUNIT_TEST(Split_Lazy_HRC_less)
            CPPUNIT_TEST(Split_Lazy_HRC_cmpmix)

            CPPUNIT_TEST(Split_Lazy_nogc_cmp)
            CPPUNIT_TEST(Split_Lazy_nogc_less)
            CPPUNIT_TEST(Split_Lazy_nogc_cmpmix)

        CPPUNIT_TEST_SUITE_END()

    };
}   // namespace map
