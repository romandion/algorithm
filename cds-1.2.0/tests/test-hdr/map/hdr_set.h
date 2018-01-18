/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "cppunit/cppunit_proxy.h"
#include <cds/container/michael_set_base.h>

namespace map {
    namespace cc = cds::container   ;
    namespace co = cds::opt         ;

    // MichaelHashSet based on MichaelList
    class HashSetHdrTest: public CppUnitMini::TestCase
    {
    public:
        struct stat
        {
            unsigned int nFindCount     ;   // count of find-functor calling
            unsigned int nEnsureNewCount;
            unsigned int nEnsureCount   ;

            stat()
            {
                memset( this, 0, sizeof(*this)) ;
            }
        };

        struct item: public stat
        {
            int nKey    ;
            int nVal    ;

            item()
            {}

            item( int key )
                : nKey( key )
                , nVal( key )
            {}

            item( const std::pair<int,int>& p )
                : nKey( p.first )
                , nVal( p.second )
            {}

            int key() const
            {
                return nKey ;
            }

            int val() const
            {
                return nVal ;
            }
        };

        struct hash_int {
            size_t operator()( int i ) const
            {
                return co::v::hash<int>()( i ) ;
            }

            size_t operator()( std::pair<int,int> const& i ) const
            {
                return co::v::hash<int>()( i.first ) ;
            }

            template <typename Item>
            size_t operator()( const Item& i ) const
            {
                return (*this)( i.key() ) ;
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

        struct set_traits: public cc::michael_set::type_traits
        {
            typedef hash_int            hash            ;
            typedef simple_item_counter item_counter    ;
        };


        template <typename T>
        struct less
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() < v2.key()  ;
            }

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() < v2  ;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 < v2.key()  ;
            }
        };

        template <typename T>
        struct cmp {
            int operator ()(const T& v1, const T& v2 ) const
            {
                if ( v1.key() < v2.key() )
                    return -1   ;
                return v1.key() > v2.key() ? 1 : 0  ;
            }

            template <typename Q>
            int operator ()(const T& v1, const Q& v2 ) const
            {
                if ( v1.key() < v2 )
                    return -1   ;
                return v1.key() > v2 ? 1 : 0  ;
            }

            template <typename Q>
            int operator ()(const Q& v1, const T& v2 ) const
            {
                if ( v1 < v2.key() )
                    return -1   ;
                return v1 > v2.key() ? 1 : 0  ;
            }
        };

        struct find_functor
        {
            template <typename Item, typename T>
            void operator()( Item& i, T& val )
            {
                ++i.nFindCount   ;
            }
        };

        template <typename Item>
        struct copy_found
        {
            Item    m_found   ;

            template <typename T>
            void operator()( Item& i, T& /*val*/ )
            {
                m_found = i   ;
            }

            void operator()( Item const& i )
            {
                m_found = i   ;
            }
        };

        struct insert_functor
        {
            template <typename Item>
            void operator()(Item& i )
            {
                i.nVal = i.nKey * 100 ;
            }
        };

        template <typename Item, typename Q>
        static void ensure_func( bool bNew, Item& i, Q& /*val*/ )
        {
            if ( bNew )
                ++i.nEnsureNewCount  ;
            else
                ++i.nEnsureCount     ;
        }

        struct ensure_functor
        {
            template <typename Item, typename Q>
            void operator()( bool bNew, Item& i, Q& val )
            {
                ensure_func( bNew, i, val ) ;
            }
        };


        template <class SET>
        void test_int()
        {
            typedef typename SET::value_type    value_type  ;

            SET s( 50, 4 )  ;
            item itm    ;
            int key     ;

            // insert/find test
            CPPUNIT_ASSERT( !s.find( 10 ) )  ;
            CPPUNIT_ASSERT( s.insert( 10 )) ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 1 ) ;
            CPPUNIT_ASSERT( s.find( 10 ) )  ;

            CPPUNIT_ASSERT( !s.insert( 10 )) ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 1 ) ;

            CPPUNIT_ASSERT( !s.find( 20 ) )  ;
            CPPUNIT_ASSERT( s.insert( std::make_pair(20, 25) )) ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 2 ) ;
            CPPUNIT_ASSERT( s.find( 10 ) )  ;
            CPPUNIT_ASSERT( s.find( key = 20 ) )  ;
            CPPUNIT_ASSERT( s.find( key, find_functor() ) )  ;
            {
                copy_found<item> f  ;
                key = 20    ;
                CPPUNIT_ASSERT( s.find( key, boost::ref(f) ) )  ;
                CPPUNIT_ASSERT( f.m_found.nKey == 20 )          ;
                CPPUNIT_ASSERT( f.m_found.nVal == 25 )          ;
                CPPUNIT_ASSERT( f.m_found.nFindCount == 1 )     ;
            }
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 2 ) ;

            CPPUNIT_ASSERT( !s.find( 25 ) )  ;
            CPPUNIT_ASSERT( s.insert( std::make_pair(25, -1), insert_functor() )) ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 3 ) ;
            {
                copy_found<item> f  ;
                key = 25    ;
                CPPUNIT_ASSERT( s.find( key, boost::ref(f) ) )  ;
                CPPUNIT_ASSERT( f.m_found.nKey == 25 )          ;
                CPPUNIT_ASSERT( f.m_found.nVal == 2500 )        ;
            }

            // ensure test
            key = 10    ;
            {
                copy_found<item> f  ;
                CPPUNIT_ASSERT( s.find( key, boost::ref(f) ) )      ;
                CPPUNIT_ASSERT( f.m_found.nKey == 10 )              ;
                CPPUNIT_ASSERT( f.m_found.nVal == 10 )              ;
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 0 )       ;
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 0 )    ;
            }
            std::pair<bool, bool> ensureResult = s.ensure( key, ensure_functor() )   ;
            CPPUNIT_ASSERT( ensureResult.first && !ensureResult.second )    ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 3 ) ;
            {
                copy_found<item> f  ;
                CPPUNIT_ASSERT( s.find( key, boost::ref(f) ) )  ;
                CPPUNIT_ASSERT( f.m_found.nKey == 10 )          ;
                CPPUNIT_ASSERT( f.m_found.nVal == 10 )        ;
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 1 )       ;
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 0 )    ;
            }

            ensureResult = s.ensure( std::make_pair(13, 1300), ensure_functor() )   ;
            CPPUNIT_ASSERT( ensureResult.first && ensureResult.second )    ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 4 ) ;
            {
                copy_found<item> f  ;
                key = 13    ;
                CPPUNIT_ASSERT( s.find( key, boost::ref(f) ) )  ;
                CPPUNIT_ASSERT( f.m_found.nKey == 13 )          ;
                CPPUNIT_ASSERT( f.m_found.nVal == 1300 )        ;
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 0 )       ;
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 1 )    ;
            }

            // erase/extract test
            CPPUNIT_ASSERT( s.erase(13) )   ;
            CPPUNIT_ASSERT( !s.find( 13 ))  ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 3 ) ;
            CPPUNIT_ASSERT( !s.erase(13) )  ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 3 ) ;

            CPPUNIT_ASSERT( s.find( 10 ))  ;
            CPPUNIT_ASSERT( s.erase( 10 )) ;
            CPPUNIT_ASSERT( !s.find( 10 ))  ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 2 ) ;
            CPPUNIT_ASSERT( !s.erase(10) )  ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 2 ) ;

            CPPUNIT_ASSERT( s.find(20) )    ;
            {
                copy_found<item> f  ;
                CPPUNIT_ASSERT( s.erase( 20, boost::ref(f) )) ;
                CPPUNIT_ASSERT( f.m_found.nKey == 20 )          ;
                CPPUNIT_ASSERT( f.m_found.nVal == 25 )        ;
            }
            CPPUNIT_ASSERT( !s.find( 20 ))  ;
            CPPUNIT_ASSERT( !s.empty() )    ;
            CPPUNIT_ASSERT( s.size() == 1 ) ;

            s.clear()   ;
            CPPUNIT_ASSERT( s.empty() )     ;
            CPPUNIT_ASSERT( s.size() == 0 ) ;

            // iterator test
            test_iter<SET>() ;
        }


        template <class SET>
        void test_int_nogc()
        {
            typedef typename SET::value_type        value_type  ;
            typedef typename SET::iterator          iterator    ;
            typedef typename SET::const_iterator    const_iterator  ;

            {
                SET s( 52, 4 )  ;
                iterator it     ;

                CPPUNIT_ASSERT( s.empty() )     ;
                CPPUNIT_ASSERT( s.size() == 0 ) ;

                // insert
                it = s.insert( 10 ) ;
                CPPUNIT_ASSERT( it != s.end() ) ;
                CPPUNIT_ASSERT( it->key() == 10 )   ;
                CPPUNIT_ASSERT( it->val() == 10 )   ;
                CPPUNIT_ASSERT( !s.empty() )        ;
                CPPUNIT_ASSERT( s.size() == 1 )     ;
                CPPUNIT_ASSERT( s.insert( 10 ) == s.end() ) ;

                it = s.insert( std::make_pair( 50, 25 )) ;
                CPPUNIT_ASSERT( it != s.end() ) ;
                CPPUNIT_ASSERT( it->key() == 50 )   ;
                CPPUNIT_ASSERT( it->val() == 25 )   ;
                CPPUNIT_ASSERT( !s.empty() )        ;
                CPPUNIT_ASSERT( s.size() == 2 )     ;
                CPPUNIT_ASSERT( s.insert( 50 ) == s.end() ) ;

                // ensure
                std::pair< iterator, bool>  ensureResult    ;
                ensureResult = s.ensure( 20 )   ;
                CPPUNIT_ASSERT( ensureResult.first != s.end() ) ;
                CPPUNIT_ASSERT( ensureResult.second  ) ;
                CPPUNIT_ASSERT( ensureResult.first->key() == 20 )   ;
                CPPUNIT_ASSERT( ensureResult.first->val() == 20 )   ;
                CPPUNIT_ASSERT( !s.empty() )        ;
                CPPUNIT_ASSERT( s.size() == 3 )     ;

                ensureResult = s.ensure( std::make_pair( 20, 200 ))   ;
                CPPUNIT_ASSERT( ensureResult.first != s.end() ) ;
                CPPUNIT_ASSERT( !ensureResult.second  ) ;
                CPPUNIT_ASSERT( ensureResult.first->key() == 20 )   ;
                CPPUNIT_ASSERT( ensureResult.first->val() == 20 )   ;
                CPPUNIT_ASSERT( !s.empty() )        ;
                CPPUNIT_ASSERT( s.size() == 3 )     ;
                ensureResult.first->nVal = 22      ;

                ensureResult = s.ensure( std::make_pair( 30, 33 ))   ;
                CPPUNIT_ASSERT( ensureResult.first != s.end() ) ;
                CPPUNIT_ASSERT( ensureResult.second  ) ;
                CPPUNIT_ASSERT( ensureResult.first->key() == 30 )   ;
                CPPUNIT_ASSERT( ensureResult.first->val() == 33 )   ;
                CPPUNIT_ASSERT( !s.empty() )        ;
                CPPUNIT_ASSERT( s.size() == 4 )     ;

                // find
                it = s.find( 10 )   ;
                CPPUNIT_ASSERT( it != s.end() ) ;
                CPPUNIT_ASSERT( it->key() == 10 )   ;
                CPPUNIT_ASSERT( it->val() == 10 )   ;

                it = s.find( 20 )   ;
                CPPUNIT_ASSERT( it != s.end() ) ;
                CPPUNIT_ASSERT( it->key() == 20 )   ;
                CPPUNIT_ASSERT( it->val() == 22 )   ;

                it = s.find( 30 )   ;
                CPPUNIT_ASSERT( it != s.end() ) ;
                CPPUNIT_ASSERT( it->key() == 30 )   ;
                CPPUNIT_ASSERT( it->val() == 33 )   ;

                it = s.find( 40 )   ;
                CPPUNIT_ASSERT( it == s.end() ) ;

                it = s.find( 50 )   ;
                CPPUNIT_ASSERT( it != s.end() ) ;
                CPPUNIT_ASSERT( it->key() == 50 )   ;
                CPPUNIT_ASSERT( it->val() == 25 )   ;

                //s.clear()   ;
                //CPPUNIT_ASSERT( s.empty() )     ;
                //CPPUNIT_ASSERT( s.size() == 0 ) ;
            }

            {
                SET s( 52, 4 )  ;

                // iterator test
                for ( int i = 0; i < 500; ++i ) {
                    CPPUNIT_ASSERT( s.insert( std::make_pair( i, i * 2) ) != s.end() )   ;
                }
                for ( iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal )    ;
                    it->nVal = (*it).nKey   ;
                }

                SET const& refSet = s   ;
                for ( const_iterator it = refSet.begin(), itEnd = refSet.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey == it->nVal )    ;
                }
            }
        }

        template <class SET>
        void test_iter()
        {
            typedef typename SET::value_type        value_type  ;
            typedef typename SET::iterator          iterator ;
            typedef typename SET::const_iterator    const_iterator ;

            SET s( 100, 4 )     ;

            const size_t nMaxCount = 500 ;
            for ( int i = 0; size_t(i) < nMaxCount; ++i ) {
                CPPUNIT_ASSERT( s.insert( std::make_pair( i, i * 2) ))   ;
            }
            size_t nCount = 0   ;
            for ( iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT_EX( (*it).nKey * 2 == it->nVal,
                    "Step " << nCount << ": Iterator key=" << it->nKey <<  ", value expected=" << it->nKey * 2 << ", value real=" << it->nVal
                    )    ;
                it->nVal = (*it).nKey   ;
                ++nCount    ;
            }
            CPPUNIT_ASSERT( nCount == nMaxCount )   ;

            nCount = 0 ;
            SET const& refSet = s;
            for ( const_iterator it = refSet.begin(), itEnd = refSet.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT_EX( (*it).nKey == it->nVal,
                    "Step " << nCount << ": Iterator key=" << it->nKey <<  ", value expected=" << it->nKey << ", value real=" << it->nVal
                    );
                ++nCount    ;
            }
            CPPUNIT_ASSERT( nCount == nMaxCount )   ;
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


        CPPUNIT_TEST_SUITE(HashSetHdrTest)
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

} // namespace map


