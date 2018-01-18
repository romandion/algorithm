/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "cppunit/cppunit_proxy.h"
#include <cds/intrusive/lazy_list_base.h>

namespace ordlist {
    namespace ci = cds::intrusive   ;
    namespace co = cds::opt         ;

    class IntrusiveLazyListHeaderTest: public CppUnitMini::TestCase
    {
    public:

        struct stat {
            int nDisposeCount   ;
            int nEnsureExistsCall;
            int nEnsureNewCall  ;
            int nFindCall       ;

            stat()
                : nDisposeCount(0)
                , nEnsureExistsCall(0)
                , nEnsureNewCall(0)
                , nFindCall(0)
            {}

            stat( const stat& s )
            {
                *this = s ;
            }

            stat& operator =(const stat& s)
            {
                memcpy( this, &s, sizeof(s)) ;
                return *this ;
            }
        };

        template <typename GC>
        struct base_int_item: public ci::lazy_list::node< GC >
        {
            int nKey            ;
            int nVal            ;

            stat    s   ;

            base_int_item()
            {}

            base_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
                , s()
            {}

            base_int_item(const base_int_item& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
                , s()
            {}

            const int& key() const
            {
                return nKey ;
            }
        };

        template <typename GC>
        struct member_int_item
        {
            int nKey            ;
            int nVal            ;

            ci::lazy_list::node< GC > hMember ;

            stat s  ;

            member_int_item()
            {}

            member_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
                , s()
            {}

            member_int_item(const member_int_item& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
                , s()
            {}

            const int& key() const
            {
                return nKey ;
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
                ++p->s.nDisposeCount  ;
            }
        };

        struct ensure_functor
        {
            template <typename T>
            void operator ()(bool bNew, T& item, T& val )
            {
                if ( bNew )
                    ++item.s.nEnsureNewCall  ;
                else
                    ++item.s.nEnsureExistsCall    ;
            }
        };

        struct find_functor
        {
            template <typename T, typename Q>
            void operator ()( T& item, Q& val )
            {
                ++item.s.nFindCall  ;
            }
        };

        template <class ORDLIST>
        void test_int()
        {
            typedef typename ORDLIST::value_type    value_type  ;

            value_type v1( 10, 50 ) ;
            value_type v2( 5, 25  ) ;
            value_type v3( 20, 100 );
            {
                ORDLIST l   ;
                CPPUNIT_ASSERT( l.empty() ) ;

                CPPUNIT_ASSERT( l.insert( v1 ))     ;   // true
                CPPUNIT_ASSERT( l.find( v1.key() )) ;

                CPPUNIT_ASSERT( v1.s.nFindCall == 0 ) ;
                CPPUNIT_ASSERT( l.find( v1.key(), find_functor() )) ;
                CPPUNIT_ASSERT( v1.s.nFindCall == 1 ) ;

                CPPUNIT_ASSERT( !l.find( v2.key() )) ;
                CPPUNIT_ASSERT( !l.find( v3.key() )) ;
                CPPUNIT_ASSERT( !l.empty() )        ;

                //CPPUNIT_ASSERT( !l.insert( v1 ))    ;   // assertion "is_empty" is raised

                {
                    value_type v( v1 )  ;
                    CPPUNIT_ASSERT( !l.insert( v )) ;   // false
                }

                std::pair<bool, bool> ret = l.ensure( v2, ensure_functor() )    ;
                CPPUNIT_ASSERT( ret.first )     ;
                CPPUNIT_ASSERT( ret.second )    ;
                CPPUNIT_ASSERT( v2.s.nEnsureNewCall == 1 )   ;
                CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 0 )   ;

                //CPPUNIT_ASSERT( !l.insert( v2 ))    ;   // assertion "is_empty"

                CPPUNIT_ASSERT( l.find( v1.key() )) ;   // true

                CPPUNIT_ASSERT( v1.s.nFindCall == 1 ) ;
                CPPUNIT_ASSERT( l.find( v1.key(), find_functor() )) ;
                CPPUNIT_ASSERT( v1.s.nFindCall == 2 ) ;

                CPPUNIT_ASSERT( l.find( v2.key() )) ;

                CPPUNIT_ASSERT( v2.s.nFindCall == 0 ) ;
                CPPUNIT_ASSERT( l.find( v2.key(), find_functor() )) ;
                CPPUNIT_ASSERT( v2.s.nFindCall == 1 ) ;

                CPPUNIT_ASSERT( !l.find( v3.key() )) ;

                {
                    CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 0 )   ;
                    CPPUNIT_ASSERT( v2.s.nEnsureNewCall == 1 )   ;

                    value_type v( v2 )  ;
                    ret = l.ensure( v, ensure_functor() )    ;

                    CPPUNIT_ASSERT( ret.first )             ;
                    CPPUNIT_ASSERT( !ret.second )           ;
                    CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 1 )   ;
                    CPPUNIT_ASSERT( v2.s.nEnsureNewCall == 1 )   ;
                    CPPUNIT_ASSERT( v.s.nEnsureExistsCall == 0 )   ;
                    CPPUNIT_ASSERT( v.s.nEnsureNewCall == 0 )   ;
                }

                CPPUNIT_ASSERT( !l.empty() ) ;

                CPPUNIT_ASSERT( l.insert( v3 ))     ;   // true
                CPPUNIT_ASSERT( l.find( v3.key() )) ;

                CPPUNIT_ASSERT( v3.s.nFindCall == 0 ) ;
                CPPUNIT_ASSERT( l.find( v3.key(), find_functor() )) ;
                CPPUNIT_ASSERT( v3.s.nFindCall == 1 ) ;

                CPPUNIT_ASSERT( l.unlink( v2 ) )    ;
                CPPUNIT_ASSERT( l.find( v1.key() )) ;   // true
                CPPUNIT_ASSERT( !l.find( v2.key() )) ;   // true
                CPPUNIT_ASSERT( l.find( v3.key() )) ;   // true
                CPPUNIT_ASSERT( !l.empty() ) ;
                CPPUNIT_ASSERT( !l.unlink( v2 ) )    ;

                {
                    // v1 key is in the list but v NODE is not in the list
                    value_type v( v1 )  ;
                    CPPUNIT_ASSERT( !l.unlink( v ) )    ;
                }

                CPPUNIT_ASSERT( l.unlink( v1 ) )    ;
                CPPUNIT_ASSERT( !l.unlink( v1 ) )    ;
                CPPUNIT_ASSERT( !l.find( v1.key() )) ;
                CPPUNIT_ASSERT( !l.find( v2.key() )) ;
                CPPUNIT_ASSERT( l.find( v3.key() )) ;
                CPPUNIT_ASSERT( !l.empty() ) ;
                CPPUNIT_ASSERT( !l.unlink( v1 ) )        ;
                CPPUNIT_ASSERT( !l.unlink( v2 ) )       ;

                CPPUNIT_ASSERT( l.unlink( v3 ) )    ;
                CPPUNIT_ASSERT( !l.find( v1.key() )) ;
                CPPUNIT_ASSERT( !l.find( v2.key() )) ;
                CPPUNIT_ASSERT( !l.find( v3.key() )) ;
                CPPUNIT_ASSERT( l.empty() )         ;
                CPPUNIT_ASSERT( !l.unlink( v1 ) )   ;
                CPPUNIT_ASSERT( !l.unlink( v2 ) )   ;
                CPPUNIT_ASSERT( !l.unlink( v3 ) )   ;

                // Apply retired pointer to clean links
                ORDLIST::gc::scan() ;

                stat s( v3.s )  ;
                ret = l.ensure( v3, ensure_functor() )  ;
                CPPUNIT_ASSERT( ret.first )     ;
                CPPUNIT_ASSERT( ret.second )    ;
                CPPUNIT_ASSERT( v3.s.nEnsureNewCall == s.nEnsureNewCall + 1)   ;
                CPPUNIT_ASSERT( v3.s.nEnsureExistsCall == s.nEnsureExistsCall )   ;
                CPPUNIT_ASSERT( !l.empty() )         ;

                s = v2.s    ;
                ret = l.ensure( v2, ensure_functor() )  ;
                CPPUNIT_ASSERT( ret.first )     ;
                CPPUNIT_ASSERT( ret.second )    ;
                CPPUNIT_ASSERT( v2.s.nEnsureNewCall == s.nEnsureNewCall + 1)   ;
                CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == s.nEnsureExistsCall )   ;
                CPPUNIT_ASSERT( !l.empty() )         ;

                s = v1.s    ;
                ret = l.ensure( v1, ensure_functor() )  ;
                CPPUNIT_ASSERT( ret.first )     ;
                CPPUNIT_ASSERT( ret.second )    ;
                CPPUNIT_ASSERT( v1.s.nEnsureNewCall == s.nEnsureNewCall + 1)   ;
                CPPUNIT_ASSERT( v1.s.nEnsureExistsCall == s.nEnsureExistsCall )   ;
                CPPUNIT_ASSERT( !l.empty() )         ;

                // Erase test
                CPPUNIT_ASSERT( l.erase( v1.key()) ) ;
                //CPPUNIT_ASSERT( v1.s.nDisposeCount == 0 )   ;
                CPPUNIT_ASSERT( !l.empty() )         ;

                CPPUNIT_ASSERT( l.erase( v2.key()) ) ;
                CPPUNIT_ASSERT( !l.erase( v2.key())) ;
                //CPPUNIT_ASSERT( v2.s.nDisposeCount == 0 )   ;
                CPPUNIT_ASSERT( !l.empty() )         ;

                CPPUNIT_ASSERT( !l.erase( v2 )) ;
                CPPUNIT_ASSERT( !l.erase( v1 )) ;
                //CPPUNIT_ASSERT( v2.s.nDisposeCount == 0 )   ;
                CPPUNIT_ASSERT( !l.empty() )         ;

                CPPUNIT_ASSERT( l.erase( v3 )) ;
                //CPPUNIT_ASSERT( v3.s.nDisposeCount == 0 )   ;
                CPPUNIT_ASSERT( l.empty() )         ;

                // Apply retired pointer to clean links
                ORDLIST::gc::scan() ;

                // Unlink test
                CPPUNIT_ASSERT( l.insert( v1 ))         ;
                CPPUNIT_ASSERT( l.insert( v3 ))         ;
                CPPUNIT_ASSERT( !l.empty() )            ;
                CPPUNIT_ASSERT( !l.unlink( v2 ))  ;
                CPPUNIT_ASSERT( l.unlink( v1 ))   ;
                CPPUNIT_ASSERT( !l.unlink( v1 ))   ;
                CPPUNIT_ASSERT( l.unlink( v3 ))        ;
                CPPUNIT_ASSERT( !l.unlink( v3 ))        ;
                CPPUNIT_ASSERT( l.empty() )             ;

                // Apply retired pointer
                ORDLIST::gc::scan() ;
                CPPUNIT_ASSERT( v1.s.nDisposeCount == 3 )   ;
                CPPUNIT_ASSERT( v2.s.nDisposeCount == 2 )   ;
                CPPUNIT_ASSERT( v3.s.nDisposeCount == 3 )   ;

                // Destructor test (call disposer)
                CPPUNIT_ASSERT( l.insert( v1 ))     ;
                CPPUNIT_ASSERT( l.insert( v3 ))     ;
                CPPUNIT_ASSERT( l.insert( v2 ))     ;

                // Iterator test
                {
                    typename ORDLIST::iterator it = l.begin()        ;
                    CPPUNIT_ASSERT( it != l.end() )         ;
                    CPPUNIT_ASSERT( it->nKey == v2.nKey )   ;
                    CPPUNIT_ASSERT( it->nVal == v2.nVal )   ;
                    CPPUNIT_ASSERT( ++it != l.end() )       ;
                    CPPUNIT_ASSERT( it->nKey == v1.nKey )   ;
                    CPPUNIT_ASSERT( it->nVal == v1.nVal )   ;
                    CPPUNIT_ASSERT( ++it != l.end() )       ;
                    CPPUNIT_ASSERT( it->nKey == v3.nKey )   ;
                    CPPUNIT_ASSERT( it->nVal == v3.nVal )   ;
                    CPPUNIT_ASSERT( ++it == l.end() )       ;
                }

                {
                    ORDLIST const & lref = l    ;
                    typename ORDLIST::const_iterator it = lref.begin()  ;
                    CPPUNIT_ASSERT( it != l.end() )         ;
                    CPPUNIT_ASSERT( it->nKey == v2.nKey )   ;
                    CPPUNIT_ASSERT( it->nVal == v2.nVal )   ;
                    CPPUNIT_ASSERT( ++it != lref.end() )    ;
                    CPPUNIT_ASSERT( it->nKey == v1.nKey )   ;
                    CPPUNIT_ASSERT( it->nVal == v1.nVal )   ;
                    CPPUNIT_ASSERT( ++it != l.end() )       ;
                    CPPUNIT_ASSERT( it->nKey == v3.nKey )   ;
                    CPPUNIT_ASSERT( it->nVal == v3.nVal )   ;
                    CPPUNIT_ASSERT( ++it == l.end() )       ;
                }
            }

            // Apply retired pointer
            ORDLIST::gc::scan() ;

            CPPUNIT_ASSERT( v1.s.nDisposeCount == 4 ) ;
            CPPUNIT_ASSERT( v2.s.nDisposeCount == 3 ) ;
            CPPUNIT_ASSERT( v3.s.nDisposeCount == 4 ) ;
        }

        template <class ORDLIST>
        void test_nogc_int()
        {
            typedef typename ORDLIST::value_type    value_type  ;
            {
                value_type v1( 10, 50 ) ;
                value_type v2( 5, 25  ) ;
                value_type v3( 20, 100 );
                {
                    ORDLIST l   ;
                    CPPUNIT_ASSERT( l.empty() ) ;

                    CPPUNIT_ASSERT( l.insert( v1 ))     ;   // true
                    CPPUNIT_ASSERT( l.find( v1.key() ) == &v1 ) ;

                    CPPUNIT_ASSERT( v1.s.nFindCall == 0 ) ;
                    CPPUNIT_ASSERT( l.find( v1.key(), find_functor() )) ;
                    CPPUNIT_ASSERT( v1.s.nFindCall == 1 ) ;

                    CPPUNIT_ASSERT( l.find( v2.key() ) == NULL ) ;
                    CPPUNIT_ASSERT( l.find( v3.key() ) == NULL ) ;
                    CPPUNIT_ASSERT( !l.empty() )        ;

                    //CPPUNIT_ASSERT( !l.insert( v1 ))    ;   // assertion "is_empty" is raised

                    {
                        value_type v( v1 )  ;
                        CPPUNIT_ASSERT( !l.insert( v )) ;   // false
                    }

                    std::pair<bool, bool> ret = l.ensure( v2, ensure_functor() )    ;
                    CPPUNIT_ASSERT( ret.first )     ;
                    CPPUNIT_ASSERT( ret.second )    ;
                    CPPUNIT_ASSERT( v2.s.nEnsureNewCall == 1 )   ;
                    CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 0 )   ;

                    //CPPUNIT_ASSERT( !l.insert( v2 ))    ;   // assertion "is_empty"

                    CPPUNIT_ASSERT( l.find( v1.key() ) == &v1 ) ;   // true

                    CPPUNIT_ASSERT( v1.s.nFindCall == 1 ) ;
                    CPPUNIT_ASSERT( l.find( v1.key(), find_functor() )) ;
                    CPPUNIT_ASSERT( v1.s.nFindCall == 2 ) ;

                    CPPUNIT_ASSERT( l.find( v2.key() ) == &v2 ) ;

                    CPPUNIT_ASSERT( v2.s.nFindCall == 0 ) ;
                    CPPUNIT_ASSERT( l.find( v2.key(), find_functor() )) ;
                    CPPUNIT_ASSERT( v2.s.nFindCall == 1 ) ;

                    CPPUNIT_ASSERT( !l.find( v3.key() )) ;

                    {
                        value_type v( v2 )  ;
                        ret = l.ensure( v, ensure_functor() )    ;

                        CPPUNIT_ASSERT( ret.first )             ;
                        CPPUNIT_ASSERT( !ret.second )           ;
                        CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 1 )   ;
                        CPPUNIT_ASSERT( v.s.nEnsureExistsCall == 0 && v.s.nEnsureNewCall == 0 )   ;
                    }

                    CPPUNIT_ASSERT( !l.empty() ) ;

                    CPPUNIT_ASSERT( l.insert( v3 ))     ;   // true
                    CPPUNIT_ASSERT( l.find( v3.key() ) == &v3 ) ;

                    CPPUNIT_ASSERT( v3.s.nFindCall == 0 ) ;
                    CPPUNIT_ASSERT( l.find( v3.key(), find_functor() )) ;
                    CPPUNIT_ASSERT( v3.s.nFindCall == 1 ) ;

                    {
                        typename ORDLIST::iterator it = l.begin()        ;
                        CPPUNIT_ASSERT( it != l.end() )         ;
                        CPPUNIT_ASSERT( it->nKey == v2.nKey )   ;
                        CPPUNIT_ASSERT( it->nVal == v2.nVal )   ;
                        CPPUNIT_ASSERT( ++it != l.end() )       ;
                        CPPUNIT_ASSERT( it->nKey == v1.nKey )   ;
                        CPPUNIT_ASSERT( it->nVal == v1.nVal )   ;
                        CPPUNIT_ASSERT( it++ != l.end() )       ;
                        CPPUNIT_ASSERT( it->nKey == v3.nKey )   ;
                        CPPUNIT_ASSERT( it->nVal == v3.nVal )   ;
                        CPPUNIT_ASSERT( it++ != l.end() )       ;
                        CPPUNIT_ASSERT( it == l.end() )         ;
                    }

                    {
                        ORDLIST const & lref = l    ;
                        typename ORDLIST::const_iterator it = lref.begin()  ;
                        CPPUNIT_ASSERT( it != l.end() )         ;
                        CPPUNIT_ASSERT( it->nKey == v2.nKey )   ;
                        CPPUNIT_ASSERT( it->nVal == v2.nVal )   ;
                        CPPUNIT_ASSERT( ++it != lref.end() )    ;
                        CPPUNIT_ASSERT( it->nKey == v1.nKey )   ;
                        CPPUNIT_ASSERT( it->nVal == v1.nVal )   ;
                        CPPUNIT_ASSERT( it++ != l.end() )       ;
                        CPPUNIT_ASSERT( it->nKey == v3.nKey )   ;
                        CPPUNIT_ASSERT( it->nVal == v3.nVal )   ;
                        CPPUNIT_ASSERT( it++ != lref.end() )    ;
                        CPPUNIT_ASSERT( it == l.end() )         ;
                    }
                }

                // Disposer called on list destruction
                CPPUNIT_ASSERT( v1.s.nDisposeCount == 1 ) ;
                CPPUNIT_ASSERT( v2.s.nDisposeCount == 1 ) ;
                CPPUNIT_ASSERT( v3.s.nDisposeCount == 1 ) ;
            }
        }


        void HP_base_cmp()          ;
        void HP_base_less()         ;
        void HP_base_cmpmix()       ;
        void HP_base_ic()           ;
        void HP_member_cmp()        ;
        void HP_member_less()       ;
        void HP_member_cmpmix()     ;
        void HP_member_ic()         ;

        void PTB_base_cmp()         ;
        void PTB_base_less()        ;
        void PTB_base_cmpmix()      ;
        void PTB_base_ic()          ;
        void PTB_member_cmp()       ;
        void PTB_member_less()      ;
        void PTB_member_cmpmix()    ;
        void PTB_member_ic()        ;

        void HRC_base_cmp()         ;
        void HRC_base_less()        ;
        void HRC_base_cmpmix()      ;
        void HRC_base_ic()          ;

        void nogc_base_cmp()         ;
        void nogc_base_less()        ;
        void nogc_base_cmpmix()      ;
        void nogc_base_ic()          ;
        void nogc_member_cmp()       ;
        void nogc_member_less()      ;
        void nogc_member_cmpmix()    ;
        void nogc_member_ic()        ;


        CPPUNIT_TEST_SUITE(IntrusiveLazyListHeaderTest)
            CPPUNIT_TEST(HP_base_cmp)
            CPPUNIT_TEST(HP_base_less)
            CPPUNIT_TEST(HP_base_cmpmix)
            CPPUNIT_TEST(HP_base_ic)
            CPPUNIT_TEST(HP_member_cmp)
            CPPUNIT_TEST(HP_member_less)
            CPPUNIT_TEST(HP_member_cmpmix)
            CPPUNIT_TEST(HP_member_ic)

            CPPUNIT_TEST(PTB_base_cmp)
            CPPUNIT_TEST(PTB_base_less)
            CPPUNIT_TEST(PTB_base_cmpmix)
            CPPUNIT_TEST(PTB_base_ic)
            CPPUNIT_TEST(PTB_member_cmp)
            CPPUNIT_TEST(PTB_member_less)
            CPPUNIT_TEST(PTB_member_cmpmix)
            CPPUNIT_TEST(PTB_member_ic)

            CPPUNIT_TEST(HRC_base_cmp)
            CPPUNIT_TEST(HRC_base_less)
            CPPUNIT_TEST(HRC_base_cmpmix)
            CPPUNIT_TEST(HRC_base_ic)

            CPPUNIT_TEST(nogc_base_cmp)
            CPPUNIT_TEST(nogc_base_less)
            CPPUNIT_TEST(nogc_base_cmpmix)
            CPPUNIT_TEST(nogc_base_ic)
            CPPUNIT_TEST(nogc_member_cmp)
            CPPUNIT_TEST(nogc_member_less)
            CPPUNIT_TEST(nogc_member_cmpmix)
            CPPUNIT_TEST(nogc_member_ic)

        CPPUNIT_TEST_SUITE_END()
    };
}   // namespace ordlist
