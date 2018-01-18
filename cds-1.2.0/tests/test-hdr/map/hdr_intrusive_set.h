/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "cppunit/cppunit_proxy.h"
#include <cds/intrusive/michael_list_base.h>
#include <cds/opt/hash.h>

namespace map {
    namespace ci = cds::intrusive   ;
    namespace co = cds::opt         ;

    // MichaelHashSet
    class IntrusiveHashSetHdrTest: public CppUnitMini::TestCase
    {
        struct stat
        {
            unsigned int nDisposeCount  ;   // count of disposer calling
            unsigned int nFindCount     ;   // count of find-functor calling
            unsigned int nEnsureNewCount;
            unsigned int nEnsureCount   ;

            stat()
            {
                memset( this, 0, sizeof(*this)) ;
            }
        };

        template <typename Node>
        struct base_int_item
            : public Node
            , public stat

        {
            int nKey            ;
            int nVal            ;

            base_int_item()
            {}

            base_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
            {}

            base_int_item(const base_int_item& v )
                : stat()
                , nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey ;
            }
        };

        template <typename Node>
        struct member_int_item: public stat
        {
            int nKey            ;
            int nVal            ;

            Node hMember ;

            stat s  ;

            member_int_item()
            {}

            member_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
            {}

            member_int_item(const member_int_item& v )
                : stat()
                , nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey ;
            }
        };

        struct hash_int {
            size_t operator()( int i ) const
            {
                return co::v::hash<int>()( i ) ;
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

        struct faked_disposer
        {
            template <typename T>
            void operator ()( T * p )
            {
                ++p->nDisposeCount  ;
            }
        };

        struct find_functor
        {
            template <typename Item, typename T>
            void operator()( Item& item, T& val )
            {
                ++item.nFindCount   ;
            }
        };

        struct insert_functor
        {
            template <typename Item>
            void operator()(Item& item )
            {
                item.nVal = item.nKey * 100 ;
            }
        };

        struct ensure_functor
        {
            template <typename Item>
            void operator()( bool bNew, Item& item, Item& val )
            {
                if ( bNew )
                    ++item.nEnsureNewCount  ;
                else
                    ++item.nEnsureCount     ;
            }
        };


        template <class SET>
        void test_int()
        {
            typedef typename SET::value_type    value_type  ;

            value_type v1( 10, 50 ) ;
            value_type v2( 5, 25  ) ;
            value_type v3( 20, 100 );
            int key   ;

            {
                SET s( 64, 4 )  ;

                CPPUNIT_ASSERT( s.empty() )     ;
                CPPUNIT_ASSERT( s.size() == 0 ) ;

                // insert/find test
                CPPUNIT_ASSERT( !s.find( v1.key() )) ;
                CPPUNIT_ASSERT( s.insert( v1 )) ;
                CPPUNIT_ASSERT( s.find( v1.key() )) ;
                CPPUNIT_ASSERT( s.size() == 1 ) ;
                CPPUNIT_ASSERT( !s.empty() )     ;

                CPPUNIT_ASSERT( !s.find( v2.key() )) ;
                CPPUNIT_ASSERT( s.insert( v2 )) ;
                CPPUNIT_ASSERT( v2.nFindCount == 0 )    ;
                CPPUNIT_ASSERT( s.find( key = v2.key(), find_functor() )) ;
                CPPUNIT_ASSERT( v2.nFindCount == 1 )    ;
                v2.nFindCount = 0   ;
                CPPUNIT_ASSERT( s.size() == 2 )     ;
                CPPUNIT_ASSERT( !s.empty() )        ;

                {
                    insert_functor  fi      ;
                    find_functor    ff      ;
                    CPPUNIT_ASSERT( !s.find( v3 )) ;
                    CPPUNIT_ASSERT( v3.nVal != v3.nKey * 100 )      ;
                    CPPUNIT_ASSERT( s.insert( v3, cds::ref(fi) ))    ;
                    CPPUNIT_ASSERT( v3.nVal == v3.nKey * 100 )      ;
                    CPPUNIT_ASSERT( v3.nFindCount == 0 )    ;
                    CPPUNIT_ASSERT( s.find( v3, cds::ref(ff) )) ;
                    CPPUNIT_ASSERT( v3.nFindCount == 1 )    ;
                    v3.nFindCount = 0   ;
                    CPPUNIT_ASSERT( s.size() == 3 )     ;
                    CPPUNIT_ASSERT( !s.empty() )        ;
                }

                // unlink test
                CPPUNIT_ASSERT( s.unlink( v1 ))     ;
                CPPUNIT_ASSERT( !s.find( v1 ))      ;
                CPPUNIT_ASSERT( s.find( v2 ))       ;
                CPPUNIT_ASSERT( s.find( v3 ))       ;
                CPPUNIT_ASSERT( s.size() == 2 )     ;

                CPPUNIT_ASSERT( !s.unlink( v1 ))    ;
                CPPUNIT_ASSERT( !s.find( v1 ))      ;
                CPPUNIT_ASSERT( s.size() == 2 )     ;

                CPPUNIT_ASSERT( s.find( v3 ))       ;
                CPPUNIT_ASSERT( s.unlink( v3 ))     ;
                CPPUNIT_ASSERT( !s.find( v3 ))      ;
                CPPUNIT_ASSERT( s.size() == 1 )     ;

                CPPUNIT_ASSERT( s.find( v2 ))       ;
                CPPUNIT_ASSERT( s.unlink( v2 ))     ;
                CPPUNIT_ASSERT( !s.find( v1 ))      ;
                CPPUNIT_ASSERT( !s.find( v2 ))      ;
                CPPUNIT_ASSERT( !s.find( v3 ))      ;
                CPPUNIT_ASSERT( s.size() == 0 )     ;
                CPPUNIT_ASSERT( s.empty() )         ;

                SET::gc::scan() ;
                // unlink function calls disposer
                CPPUNIT_ASSERT( v1.nDisposeCount == 1 )  ;
                CPPUNIT_ASSERT( v2.nDisposeCount == 1 )  ;
                CPPUNIT_ASSERT( v3.nDisposeCount == 1 )  ;

                // ensure test
                {
                    ensure_functor f    ;
                    std::pair<bool, bool> ret = s.ensure( v1, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( ret.second )    ;
                    CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v1.nEnsureCount == 0 )   ;
                    CPPUNIT_ASSERT( s.size() == 1 )     ;

                    ret = s.ensure( v2, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( ret.second )    ;
                    CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v2.nEnsureCount == 0 )   ;
                    CPPUNIT_ASSERT( s.size() == 2 )     ;

                    ret = s.ensure( v3, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( ret.second )    ;
                    CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v3.nEnsureCount == 0 )   ;
                    CPPUNIT_ASSERT( s.size() == 3 )     ;

                    CPPUNIT_ASSERT( s.find( v1 ))      ;
                    CPPUNIT_ASSERT( s.find( v2 ))      ;
                    CPPUNIT_ASSERT( s.find( v3 ))      ;

                    ret = s.ensure( v1, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( !ret.second )    ;
                    CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v1.nEnsureCount == 1 )   ;
                    CPPUNIT_ASSERT( s.size() == 3 )     ;

                    ret = s.ensure( v2, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( !ret.second )    ;
                    CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v2.nEnsureCount == 1 )   ;
                    CPPUNIT_ASSERT( s.size() == 3 )     ;

                    ret = s.ensure( v3, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( !ret.second )    ;
                    CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v3.nEnsureCount == 1 )   ;
                    CPPUNIT_ASSERT( s.size() == 3 )     ;

                    CPPUNIT_ASSERT( s.find( v1 ))      ;
                    CPPUNIT_ASSERT( s.find( v2 ))      ;
                    CPPUNIT_ASSERT( s.find( v3 ))      ;
                }

                // erase test
                CPPUNIT_ASSERT( s.find( v1 ))       ;
                CPPUNIT_ASSERT( s.find( v2 ))       ;
                CPPUNIT_ASSERT( s.find( v3 ))       ;
                CPPUNIT_ASSERT( s.size() == 3 )     ;
                CPPUNIT_ASSERT( !s.empty() )        ;

                CPPUNIT_ASSERT( s.erase( v1 ))  ;
                CPPUNIT_ASSERT( !s.find( v1 ))       ;
                CPPUNIT_ASSERT( s.find( v2 ))       ;
                CPPUNIT_ASSERT( s.find( v3 ))       ;
                CPPUNIT_ASSERT( s.size() == 2 )     ;
                CPPUNIT_ASSERT( !s.empty() )        ;

                CPPUNIT_ASSERT( !s.erase( v1 ))  ;
                CPPUNIT_ASSERT( !s.find( v1 ))       ;
                CPPUNIT_ASSERT( s.find( v2 ))       ;
                CPPUNIT_ASSERT( s.find( v3 ))       ;
                CPPUNIT_ASSERT( s.size() == 2 )     ;
                CPPUNIT_ASSERT( !s.empty() )        ;

                CPPUNIT_ASSERT( s.erase( v2.key() ))  ;
                CPPUNIT_ASSERT( !s.find( v1 ))      ;
                CPPUNIT_ASSERT( !s.find( v2 ))      ;
                CPPUNIT_ASSERT( s.find( v3 ))       ;
                CPPUNIT_ASSERT( s.size() == 1 )     ;
                CPPUNIT_ASSERT( !s.empty() )        ;

                CPPUNIT_ASSERT( !s.erase( v2.key() ))  ;
                CPPUNIT_ASSERT( !s.find( v1 ))      ;
                CPPUNIT_ASSERT( !s.find( v2 ))      ;
                CPPUNIT_ASSERT( s.find( v3 ))       ;
                CPPUNIT_ASSERT( s.size() == 1 )     ;
                CPPUNIT_ASSERT( !s.empty() )        ;

                CPPUNIT_ASSERT( s.erase( v3 ))  ;
                CPPUNIT_ASSERT( !s.find( v1 ))      ;
                CPPUNIT_ASSERT( !s.find( v2 ))      ;
                CPPUNIT_ASSERT( !s.find( v3 ))       ;
                CPPUNIT_ASSERT( s.size() == 0 )     ;
                CPPUNIT_ASSERT( s.empty() )         ;

                // dispose test
                SET::gc::scan() ;
                CPPUNIT_ASSERT( v1.nDisposeCount == 2 )  ;
                CPPUNIT_ASSERT( v2.nDisposeCount == 2 )  ;
                CPPUNIT_ASSERT( v3.nDisposeCount == 2 )  ;

                CPPUNIT_ASSERT( s.empty() )         ;
                CPPUNIT_ASSERT( s.insert( v1 ))     ;
                CPPUNIT_ASSERT( s.insert( v3 ))     ;
                CPPUNIT_ASSERT( s.size() == 2 )     ;
                CPPUNIT_ASSERT( !s.empty() )        ;

                {
                    value_type v( v1 )  ;
                    CPPUNIT_ASSERT( !s.unlink( v ) )   ;
                }

                CPPUNIT_ASSERT( s.unlink( v3 ) )   ;
                CPPUNIT_ASSERT( s.find( v1 ))       ;
                CPPUNIT_ASSERT( !s.find( v2 ))       ;
                CPPUNIT_ASSERT( !s.find( v3 ))      ;
                CPPUNIT_ASSERT( s.size() == 1 )     ;
                CPPUNIT_ASSERT( !s.empty() )        ;

                CPPUNIT_ASSERT( !s.unlink( v3 ))  ;
                CPPUNIT_ASSERT( s.find( v1 ))       ;
                CPPUNIT_ASSERT( !s.find( v2 ))       ;
                CPPUNIT_ASSERT( !s.find( v3 ))      ;
                CPPUNIT_ASSERT( s.size() == 1 )     ;
                CPPUNIT_ASSERT( !s.empty() )        ;

                CPPUNIT_ASSERT( s.unlink( v1 ))   ;
                CPPUNIT_ASSERT( !s.find( v1 ))      ;
                CPPUNIT_ASSERT( !s.find( v2 ))       ;
                CPPUNIT_ASSERT( !s.find( v3 ))      ;
                CPPUNIT_ASSERT( s.size() == 0 )     ;
                CPPUNIT_ASSERT( s.empty() )        ;

                CPPUNIT_ASSERT( !s.unlink( v2 ))    ;
                CPPUNIT_ASSERT( !s.find( v1 ))      ;
                CPPUNIT_ASSERT( !s.find( v2 ))      ;
                CPPUNIT_ASSERT( !s.find( v3 ))      ;
                CPPUNIT_ASSERT( s.size() == 0 )     ;
                CPPUNIT_ASSERT( s.empty() )         ;

                SET::gc::scan() ;
                CPPUNIT_ASSERT( v1.nDisposeCount == 3 )  ;
                CPPUNIT_ASSERT( v2.nDisposeCount == 2 )  ;
                CPPUNIT_ASSERT( v3.nDisposeCount == 3 )  ;

                // clear test (dispose on destruct)
                CPPUNIT_ASSERT( s.insert( v1 ))     ;
                CPPUNIT_ASSERT( s.insert( v3 ))     ;
                CPPUNIT_ASSERT( s.insert( v2 ))     ;
            }

            SET::gc::scan() ;
            CPPUNIT_ASSERT( v1.nDisposeCount == 4 )  ;
            CPPUNIT_ASSERT( v2.nDisposeCount == 3 )  ;
            CPPUNIT_ASSERT( v3.nDisposeCount == 4 )  ;

            // Iterator test
            test_iter<SET>()    ;
        }

        template <class SET>
        void test_iter()
        {
            typedef typename SET::value_type    value_type  ;
            typedef typename SET::iterator set_iterator ;
            typedef typename SET::iterator const_set_iterator ;

            value_type  v[500]  ;
            {
                SET s( 100, 4 )     ;

                for ( int i = 0; unsigned(i) < sizeof(v)/sizeof(v[0]); ++i ) {
                    v[i].nKey = i       ;
                    v[i].nVal = i * 2   ;

                    CPPUNIT_ASSERT( s.insert( v[i] ))   ;
                }

                int nCount = 0  ;
                for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal )    ;
                    it->nVal = (*it).nKey   ;
                    ++nCount    ;
                }
                CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0])) ;

                nCount = 0  ;
                for ( const_set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey == it->nVal )    ;
                    ++nCount    ;
                }
                CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0])) ;

                for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                    CPPUNIT_ASSERT( v[i].nKey == v[i].nVal )   ;
                }
            }

            SET::gc::scan()  ;

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nDisposeCount == 1 )   ;
            }
        }

        template <class SET>
        void test_int_nogc()
        {
            typedef typename SET::value_type    value_type  ;

            value_type v1( 10, 50 ) ;
            value_type v2( 5, 25  ) ;
            value_type v3( 20, 100 );
            int key   ;

            {
                {
                    SET s( 64, 4 )  ;

                    // insert test
                    CPPUNIT_ASSERT( s.empty() )     ;
                    CPPUNIT_ASSERT( s.size() == 0 ) ;

                    // insert/find test
                    CPPUNIT_ASSERT( s.find( v1.key() ) == NULL ) ;
                    CPPUNIT_ASSERT( s.insert( v1 )) ;
                    CPPUNIT_ASSERT( s.find( v1.key() ) == &v1 ) ;
                    CPPUNIT_ASSERT( s.size() == 1 ) ;
                    CPPUNIT_ASSERT( !s.empty() )     ;

                    CPPUNIT_ASSERT( s.find( v2.key() ) == NULL ) ;
                    CPPUNIT_ASSERT( s.insert( v2 )) ;
                    CPPUNIT_ASSERT( v2.nFindCount == 0 )    ;
                    CPPUNIT_ASSERT( s.find( key = v2.key(), find_functor() )) ;
                    CPPUNIT_ASSERT( v2.nFindCount == 1 )    ;
                    v2.nFindCount = 0   ;
                    CPPUNIT_ASSERT( s.size() == 2 )     ;
                    CPPUNIT_ASSERT( !s.empty() )        ;

                    {
                        find_functor    ff      ;
                        CPPUNIT_ASSERT( s.find( v3 ) == NULL )  ;
                        CPPUNIT_ASSERT( s.insert( v3 ))         ;
                        CPPUNIT_ASSERT( v3.nFindCount == 0 )    ;
                        CPPUNIT_ASSERT( s.find( v3, cds::ref(ff) )) ;
                        CPPUNIT_ASSERT( v3.nFindCount == 1 )    ;
                        v3.nFindCount = 0   ;
                        CPPUNIT_ASSERT( s.size() == 3 )     ;
                        CPPUNIT_ASSERT( !s.empty() )        ;
                    }
                }

                // s.clear()   ; // not all set supports clear() method
                CPPUNIT_ASSERT( v1.nDisposeCount == 1 )  ;
                CPPUNIT_ASSERT( v2.nDisposeCount == 1 )  ;
                CPPUNIT_ASSERT( v3.nDisposeCount == 1 )  ;


                // ensure test
                {
                    SET s( 64, 4 )  ;

                    ensure_functor f    ;
                    std::pair<bool, bool> ret = s.ensure( v1, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( ret.second )    ;
                    CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v1.nEnsureCount == 0 )   ;
                    CPPUNIT_ASSERT( s.size() == 1 )     ;

                    ret = s.ensure( v2, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( ret.second )    ;
                    CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v2.nEnsureCount == 0 )   ;
                    CPPUNIT_ASSERT( s.size() == 2 )     ;

                    ret = s.ensure( v3, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( ret.second )    ;
                    CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v3.nEnsureCount == 0 )   ;
                    CPPUNIT_ASSERT( s.size() == 3 )     ;

                    CPPUNIT_ASSERT( s.find( v1 ) == &v1 )   ;
                    CPPUNIT_ASSERT( s.find( v2 ) == &v2 )   ;
                    CPPUNIT_ASSERT( s.find( v3 ) == &v3 )   ;

                    ret = s.ensure( v1, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( !ret.second )    ;
                    CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v1.nEnsureCount == 1 )   ;
                    CPPUNIT_ASSERT( s.size() == 3 )     ;

                    ret = s.ensure( v2, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( !ret.second )    ;
                    CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v2.nEnsureCount == 1 )   ;
                    CPPUNIT_ASSERT( s.size() == 3 )     ;

                    ret = s.ensure( v3, f )   ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( !ret.second )    ;
                    CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 )   ;
                    CPPUNIT_ASSERT( v3.nEnsureCount == 1 )   ;
                    CPPUNIT_ASSERT( s.size() == 3 )     ;

                    CPPUNIT_ASSERT( s.find( v1 ) == &v1 )      ;
                    CPPUNIT_ASSERT( s.find( v2 ) == &v2 )      ;
                    CPPUNIT_ASSERT( s.find( v3 ) == &v3 )      ;
                }
            }

            // dispose on destruct test
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 )  ;
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 )  ;
            CPPUNIT_ASSERT( v3.nDisposeCount == 2 )  ;

            // Iterator test
            test_iter<SET>()    ;
        }


        // MichaelHashSet + MichaelList
        void HP_base_cmp()          ;
        void HP_base_less()         ;
        void HP_base_cmpmix()       ;
        void HP_member_cmp()        ;
        void HP_member_less()       ;
        void HP_member_cmpmix()     ;

        void PTB_base_cmp()         ;
        void PTB_base_less()        ;
        void PTB_base_cmpmix()      ;
        void PTB_member_cmp()       ;
        void PTB_member_less()      ;
        void PTB_member_cmpmix()    ;

        void HRC_base_cmp()         ;
        void HRC_base_less()        ;
        void HRC_base_cmpmix()      ;

        void nogc_base_cmp()         ;
        void nogc_base_less()        ;
        void nogc_base_cmpmix()      ;
        void nogc_member_cmp()       ;
        void nogc_member_less()      ;
        void nogc_member_cmpmix()    ;

        // MichaelHashSet + LazyList
        void HP_base_cmp_lazy()          ;
        void HP_base_less_lazy()         ;
        void HP_base_cmpmix_lazy()       ;
        void HP_member_cmp_lazy()        ;
        void HP_member_less_lazy()       ;
        void HP_member_cmpmix_lazy()     ;

        void PTB_base_cmp_lazy()         ;
        void PTB_base_less_lazy()        ;
        void PTB_base_cmpmix_lazy()      ;
        void PTB_member_cmp_lazy()       ;
        void PTB_member_less_lazy()      ;
        void PTB_member_cmpmix_lazy()    ;

        void HRC_base_cmp_lazy()         ;
        void HRC_base_less_lazy()        ;
        void HRC_base_cmpmix_lazy()      ;

        void nogc_base_cmp_lazy()         ;
        void nogc_base_less_lazy()        ;
        void nogc_base_cmpmix_lazy()      ;
        void nogc_member_cmp_lazy()       ;
        void nogc_member_less_lazy()      ;
        void nogc_member_cmpmix_lazy()    ;

        // SplitListSet + MichaelList [dyn - dynamic bucket table, st - static bucket table]
        void split_dyn_HP_base_cmp()            ;
        void split_dyn_HP_base_less()           ;
        void split_dyn_HP_base_cmpmix()         ;
        void split_dyn_HP_member_cmp()          ;
        void split_dyn_HP_member_less()         ;
        void split_dyn_HP_member_cmpmix()       ;
        void split_st_HP_base_cmp()            ;
        void split_st_HP_base_less()           ;
        void split_st_HP_base_cmpmix()         ;
        void split_st_HP_member_cmp()          ;
        void split_st_HP_member_less()         ;
        void split_st_HP_member_cmpmix()       ;

        void split_dyn_PTB_base_cmp()            ;
        void split_dyn_PTB_base_less()           ;
        void split_dyn_PTB_base_cmpmix()         ;
        void split_dyn_PTB_member_cmp()          ;
        void split_dyn_PTB_member_less()         ;
        void split_dyn_PTB_member_cmpmix()       ;
        void split_st_PTB_base_cmp()            ;
        void split_st_PTB_base_less()           ;
        void split_st_PTB_base_cmpmix()         ;
        void split_st_PTB_member_cmp()          ;
        void split_st_PTB_member_less()         ;
        void split_st_PTB_member_cmpmix()       ;

        void split_dyn_HRC_base_cmp()           ;
        void split_dyn_HRC_base_less()          ;
        void split_dyn_HRC_base_cmpmix()        ;
        void split_st_HRC_base_cmp()            ;
        void split_st_HRC_base_less()           ;
        void split_st_HRC_base_cmpmix()         ;

        void split_dyn_nogc_base_cmp()         ;
        void split_dyn_nogc_base_less()        ;
        void split_dyn_nogc_base_cmpmix()      ;
        void split_dyn_nogc_member_cmp()       ;
        void split_dyn_nogc_member_less()      ;
        void split_dyn_nogc_member_cmpmix()    ;
        void split_st_nogc_base_cmp()         ;
        void split_st_nogc_base_less()        ;
        void split_st_nogc_base_cmpmix()      ;
        void split_st_nogc_member_cmp()       ;
        void split_st_nogc_member_less()      ;
        void split_st_nogc_member_cmpmix()    ;


        // SplitListSet + LazyList [dyn - dynamic bucket table, st - static bucket table]
        void split_dyn_HP_base_cmp_lazy()          ;
        void split_dyn_HP_base_less_lazy()         ;
        void split_dyn_HP_base_cmpmix_lazy()       ;
        void split_dyn_HP_member_cmp_lazy()        ;
        void split_dyn_HP_member_less_lazy()       ;
        void split_dyn_HP_member_cmpmix_lazy()     ;
        void split_st_HP_base_cmp_lazy()          ;
        void split_st_HP_base_less_lazy()         ;
        void split_st_HP_base_cmpmix_lazy()       ;
        void split_st_HP_member_cmp_lazy()        ;
        void split_st_HP_member_less_lazy()       ;
        void split_st_HP_member_cmpmix_lazy()     ;

        void split_dyn_PTB_base_cmp_lazy()         ;
        void split_dyn_PTB_base_less_lazy()        ;
        void split_dyn_PTB_base_cmpmix_lazy()      ;
        void split_dyn_PTB_member_cmp_lazy()       ;
        void split_dyn_PTB_member_less_lazy()      ;
        void split_dyn_PTB_member_cmpmix_lazy()    ;
        void split_st_PTB_base_cmp_lazy()         ;
        void split_st_PTB_base_less_lazy()        ;
        void split_st_PTB_base_cmpmix_lazy()      ;
        void split_st_PTB_member_cmp_lazy()       ;
        void split_st_PTB_member_less_lazy()      ;
        void split_st_PTB_member_cmpmix_lazy()    ;

        void split_dyn_HRC_base_cmp_lazy()         ;
        void split_dyn_HRC_base_less_lazy()        ;
        void split_dyn_HRC_base_cmpmix_lazy()      ;
        void split_st_HRC_base_cmp_lazy()         ;
        void split_st_HRC_base_less_lazy()        ;
        void split_st_HRC_base_cmpmix_lazy()      ;

        void split_dyn_nogc_base_cmp_lazy()         ;
        void split_dyn_nogc_base_less_lazy()        ;
        void split_dyn_nogc_base_cmpmix_lazy()      ;
        void split_dyn_nogc_member_cmp_lazy()       ;
        void split_dyn_nogc_member_less_lazy()      ;
        void split_dyn_nogc_member_cmpmix_lazy()    ;
        void split_st_nogc_base_cmp_lazy()         ;
        void split_st_nogc_base_less_lazy()        ;
        void split_st_nogc_base_cmpmix_lazy()      ;
        void split_st_nogc_member_cmp_lazy()       ;
        void split_st_nogc_member_less_lazy()      ;
        void split_st_nogc_member_cmpmix_lazy()    ;

        CPPUNIT_TEST_SUITE(IntrusiveHashSetHdrTest)
            CPPUNIT_TEST(HP_base_cmp)
            CPPUNIT_TEST(HP_base_less)
            CPPUNIT_TEST(HP_base_cmpmix)
            CPPUNIT_TEST(HP_member_cmp)
            CPPUNIT_TEST(HP_member_less)
            CPPUNIT_TEST(HP_member_cmpmix)

            CPPUNIT_TEST(PTB_base_cmp)
            CPPUNIT_TEST(PTB_base_less)
            CPPUNIT_TEST(PTB_base_cmpmix)
            CPPUNIT_TEST(PTB_member_cmp)
            CPPUNIT_TEST(PTB_member_less)
            CPPUNIT_TEST(PTB_member_cmpmix)

            CPPUNIT_TEST(HRC_base_cmp)
            CPPUNIT_TEST(HRC_base_less)
            CPPUNIT_TEST(HRC_base_cmpmix)

            CPPUNIT_TEST(nogc_base_cmp)
            CPPUNIT_TEST(nogc_base_less)
            CPPUNIT_TEST(nogc_base_cmpmix)
            CPPUNIT_TEST(nogc_member_cmp)
            CPPUNIT_TEST(nogc_member_less)
            CPPUNIT_TEST(nogc_member_cmpmix)

            CPPUNIT_TEST(HP_base_cmp_lazy)
            CPPUNIT_TEST(HP_base_less_lazy)
            CPPUNIT_TEST(HP_base_cmpmix_lazy)
            CPPUNIT_TEST(HP_member_cmp_lazy)
            CPPUNIT_TEST(HP_member_less_lazy)
            CPPUNIT_TEST(HP_member_cmpmix_lazy)

            CPPUNIT_TEST(PTB_base_cmp_lazy)
            CPPUNIT_TEST(PTB_base_less_lazy)
            CPPUNIT_TEST(PTB_base_cmpmix_lazy)
            CPPUNIT_TEST(PTB_member_cmp_lazy)
            CPPUNIT_TEST(PTB_member_less_lazy)
            CPPUNIT_TEST(PTB_member_cmpmix_lazy)

            CPPUNIT_TEST(HRC_base_cmp_lazy)
            CPPUNIT_TEST(HRC_base_less_lazy)
            CPPUNIT_TEST(HRC_base_cmpmix_lazy)

            CPPUNIT_TEST(nogc_base_cmp_lazy)
            CPPUNIT_TEST(nogc_base_less_lazy)
            CPPUNIT_TEST(nogc_base_cmpmix_lazy)
            CPPUNIT_TEST(nogc_member_cmp_lazy)
            CPPUNIT_TEST(nogc_member_less_lazy)
            CPPUNIT_TEST(nogc_member_cmpmix_lazy)


            CPPUNIT_TEST(split_dyn_HP_base_cmp)
            CPPUNIT_TEST(split_dyn_HP_base_less)
            CPPUNIT_TEST(split_dyn_HP_base_cmpmix)
            CPPUNIT_TEST(split_dyn_HP_member_cmp)
            CPPUNIT_TEST(split_dyn_HP_member_less)
            CPPUNIT_TEST(split_dyn_HP_member_cmpmix)
            CPPUNIT_TEST(split_st_HP_base_cmp)
            CPPUNIT_TEST(split_st_HP_base_less)
            CPPUNIT_TEST(split_st_HP_base_cmpmix)
            CPPUNIT_TEST(split_st_HP_member_cmp)
            CPPUNIT_TEST(split_st_HP_member_less)
            CPPUNIT_TEST(split_st_HP_member_cmpmix)

            CPPUNIT_TEST(split_dyn_PTB_base_cmp)
            CPPUNIT_TEST(split_dyn_PTB_base_less)
            CPPUNIT_TEST(split_dyn_PTB_base_cmpmix)
            CPPUNIT_TEST(split_dyn_PTB_member_cmp)
            CPPUNIT_TEST(split_dyn_PTB_member_less)
            CPPUNIT_TEST(split_dyn_PTB_member_cmpmix)
            CPPUNIT_TEST(split_st_PTB_base_cmp)
            CPPUNIT_TEST(split_st_PTB_base_less)
            CPPUNIT_TEST(split_st_PTB_base_cmpmix)
            CPPUNIT_TEST(split_st_PTB_member_cmp)
            CPPUNIT_TEST(split_st_PTB_member_less)
            CPPUNIT_TEST(split_st_PTB_member_cmpmix)

            CPPUNIT_TEST(split_dyn_HRC_base_cmp)
            CPPUNIT_TEST(split_dyn_HRC_base_less)
            CPPUNIT_TEST(split_dyn_HRC_base_cmpmix)
            CPPUNIT_TEST(split_st_HRC_base_cmp)
            CPPUNIT_TEST(split_st_HRC_base_less)
            CPPUNIT_TEST(split_st_HRC_base_cmpmix)

            CPPUNIT_TEST(split_dyn_nogc_base_cmp)
            CPPUNIT_TEST(split_dyn_nogc_base_less)
            CPPUNIT_TEST(split_dyn_nogc_base_cmpmix)
            CPPUNIT_TEST(split_dyn_nogc_member_cmp)
            CPPUNIT_TEST(split_dyn_nogc_member_less)
            CPPUNIT_TEST(split_dyn_nogc_member_cmpmix)
            CPPUNIT_TEST(split_st_nogc_base_cmp)
            CPPUNIT_TEST(split_st_nogc_base_less)
            CPPUNIT_TEST(split_st_nogc_base_cmpmix)
            CPPUNIT_TEST(split_st_nogc_member_cmp)
            CPPUNIT_TEST(split_st_nogc_member_less)
            CPPUNIT_TEST(split_st_nogc_member_cmpmix)


            CPPUNIT_TEST(split_dyn_HP_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_HP_base_less_lazy)
            CPPUNIT_TEST(split_dyn_HP_base_cmpmix_lazy)
            CPPUNIT_TEST(split_dyn_HP_member_cmp_lazy)
            CPPUNIT_TEST(split_dyn_HP_member_less_lazy)
            CPPUNIT_TEST(split_dyn_HP_member_cmpmix_lazy)
            CPPUNIT_TEST(split_st_HP_base_cmp_lazy)
            CPPUNIT_TEST(split_st_HP_base_less_lazy)
            CPPUNIT_TEST(split_st_HP_base_cmpmix_lazy)
            CPPUNIT_TEST(split_st_HP_member_cmp_lazy)
            CPPUNIT_TEST(split_st_HP_member_less_lazy)
            CPPUNIT_TEST(split_st_HP_member_cmpmix_lazy)

            CPPUNIT_TEST(split_dyn_PTB_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_PTB_base_less_lazy)
            CPPUNIT_TEST(split_dyn_PTB_base_cmpmix_lazy)
            CPPUNIT_TEST(split_dyn_PTB_member_cmp_lazy)
            CPPUNIT_TEST(split_dyn_PTB_member_less_lazy)
            CPPUNIT_TEST(split_dyn_PTB_member_cmpmix_lazy)
            CPPUNIT_TEST(split_st_PTB_base_cmp_lazy)
            CPPUNIT_TEST(split_st_PTB_base_less_lazy)
            CPPUNIT_TEST(split_st_PTB_base_cmpmix_lazy)
            CPPUNIT_TEST(split_st_PTB_member_cmp_lazy)
            CPPUNIT_TEST(split_st_PTB_member_less_lazy)
            CPPUNIT_TEST(split_st_PTB_member_cmpmix_lazy)

            CPPUNIT_TEST(split_dyn_HRC_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_HRC_base_less_lazy)
            CPPUNIT_TEST(split_dyn_HRC_base_cmpmix_lazy)
            CPPUNIT_TEST(split_st_HRC_base_cmp_lazy)
            CPPUNIT_TEST(split_st_HRC_base_less_lazy)
            CPPUNIT_TEST(split_st_HRC_base_cmpmix_lazy)

            CPPUNIT_TEST(split_dyn_nogc_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_nogc_base_less_lazy)
            CPPUNIT_TEST(split_dyn_nogc_base_cmpmix_lazy)
            CPPUNIT_TEST(split_dyn_nogc_member_cmp_lazy)
            CPPUNIT_TEST(split_dyn_nogc_member_less_lazy)
            CPPUNIT_TEST(split_dyn_nogc_member_cmpmix_lazy)
            CPPUNIT_TEST(split_st_nogc_base_cmp_lazy)
            CPPUNIT_TEST(split_st_nogc_base_less_lazy)
            CPPUNIT_TEST(split_st_nogc_base_cmpmix_lazy)
            CPPUNIT_TEST(split_st_nogc_member_cmp_lazy)
            CPPUNIT_TEST(split_st_nogc_member_less_lazy)
            CPPUNIT_TEST(split_st_nogc_member_cmpmix_lazy)

        CPPUNIT_TEST_SUITE_END()

    };

}   // namespace map
