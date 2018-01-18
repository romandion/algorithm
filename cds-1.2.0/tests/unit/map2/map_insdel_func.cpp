/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map2/map_types.h"
#include "cppunit/thread.h"

#include <cds/lock/spinlock.h>
#include <vector>
#include <boost/ref.hpp>

namespace map2 {

#    define TEST_MAP(X)    void X() { test<MapTypes<key_type, value_type>::X >()    ; }

    namespace {
        static size_t  c_nMapSize = 1000000    ;  // map size
        static size_t  c_nInsertThreadCount = 4;  // count of insertion thread
        static size_t  c_nDeleteThreadCount = 4;  // count of deletion thread
        static size_t  c_nEnsureThreadCount = 4;  // count of ensure thread
        static size_t  c_nThreadPassCount = 4  ;  // pass count for each thread
        static size_t  c_nMaxLoadFactor = 8    ;  // maximum load factor
        static bool    c_bPrintGCState = true  ;
    }

    class Map_InsDel_func: public CppUnitMini::TestCase
    {
        typedef size_t  key_type    ;
        struct value_type {
            size_t      nKey        ;
            size_t      nData       ;
            CDS_ATOMIC::atomic<size_t> nEnsureCall ;
            bool        bInitialized;

            typedef cds::lock::Spinlock< cds::backoff::pause >   lock_type   ;
            mutable lock_type   m_access    ;

            value_type()
                : nKey(0)
                , nData(0)
                , nEnsureCall(0)
                , bInitialized( false )
            {}

            value_type( value_type const& s )
                : nKey(s.nKey)
                , nData(s.nData)
                , nEnsureCall(s.nEnsureCall.load(CDS_ATOMIC::memory_order_relaxed))
                , bInitialized( false )
            {}

        };

        template <class Map>
        class Inserter: public CppUnitMini::TestThread
        {
            Map&     m_Map      ;
            typedef typename Map::pair_type pair_type   ;

            virtual Inserter *    clone()
            {
                return new Inserter( *this )    ;
            }

            struct insert_functor {
                size_t nTestFunctorRef ;

                insert_functor()
                    : nTestFunctorRef(0)
                {}

                void operator()( pair_type& val )
                {
                    cds::lock::Auto< typename value_type::lock_type>    ac( val.second.m_access )  ;

                    val.second.nKey  = val.first        ;
                    val.second.nData = val.first * 8    ;

                    ++nTestFunctorRef   ;
                    val.second.bInitialized = true ;
                }
            } ;

        public:
            size_t  m_nInsertSuccess    ;
            size_t  m_nInsertFailed     ;

            size_t  m_nTestFunctorRef   ;

        public:
            Inserter( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Inserter( Inserter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_func&  getTest()
            {
                return reinterpret_cast<Map_InsDel_func&>( m_Pool.m_Test )   ;
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map   ;

                m_nInsertSuccess =
                    m_nInsertFailed =
                    m_nTestFunctorRef = 0 ;

                // func is passed by reference
                insert_functor  func    ;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            if ( rMap.insert_key( nItem, cds::ref(func) ) )
                                ++m_nInsertSuccess  ;
                            else
                                ++m_nInsertFailed   ;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = c_nMapSize; nItem > 0; --nItem ) {
                            if ( rMap.insert_key( nItem - 1, cds::ref(func) ) )
                                ++m_nInsertSuccess  ;
                            else
                                ++m_nInsertFailed   ;
                        }
                    }
                }

                m_nTestFunctorRef = func.nTestFunctorRef    ;
            }
        };

        template <class Map>
        class Ensurer: public CppUnitMini::TestThread
        {
            Map&     m_Map      ;
            typedef typename Map::pair_type pair_type   ;

            virtual Ensurer *    clone()
            {
                return new Ensurer( *this )    ;
            }

            struct ensure_functor {
                size_t  nCreated    ;
                size_t  nModified   ;

                ensure_functor()
                    : nCreated(0)
                    , nModified(0)
                {}

                void operator()( bool bNew, pair_type& val )
                {
                    cds::lock::Auto<typename value_type::lock_type>    ac( val.second.m_access )     ;
                    if ( bNew ) {
                        ++nCreated          ;
                        val.second.nKey = val.first        ;
                        val.second.nData = val.first * 8   ;
                        val.second.bInitialized = true     ;
                    }
                    else {
                        val.second.nEnsureCall.fetch_add( 1, CDS_ATOMIC::memory_order_relaxed ) ;
                        ++nModified         ;
                    }
                }
            private:
                ensure_functor(const ensure_functor& )  ;
            } ;

        public:
            size_t  m_nEnsureFailed     ;
            size_t  m_nEnsureCreated    ;
            size_t  m_nEnsureExisted    ;
            size_t  m_nFunctorCreated   ;
            size_t  m_nFunctorModified  ;

        public:
            Ensurer( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Ensurer( Ensurer& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_func&  getTest()
            {
                return reinterpret_cast<Map_InsDel_func&>( m_Pool.m_Test )   ;
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map   ;

                m_nEnsureCreated =
                    m_nEnsureExisted =
                    m_nEnsureFailed = 0 ;

                ensure_functor func ;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            std::pair<bool, bool> ret = rMap.ensure( nItem, cds::ref( func ) ) ;
                            if ( ret.first  ) {
                                if ( ret.second )
                                    ++m_nEnsureCreated  ;
                                else
                                    ++m_nEnsureExisted  ;
                            }
                            else
                                ++m_nEnsureFailed       ;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = c_nMapSize; nItem > 0; --nItem ) {
                            std::pair<bool, bool> ret = rMap.ensure( nItem - 1, cds::ref( func ) ) ;
                            if ( ret.first  ) {
                                if ( ret.second )
                                    ++m_nEnsureCreated  ;
                                else
                                    ++m_nEnsureExisted  ;
                            }
                            else
                                ++m_nEnsureFailed       ;
                        }
                    }
                }

                m_nFunctorCreated = func.nCreated   ;
                m_nFunctorModified = func.nModified ;
            }
        };

        template <class Map>
        class Deleter: public CppUnitMini::TestThread
        {
            Map&     m_Map      ;
            typedef typename Map::value_type value_type ;
            typedef typename Map::pair_type pair_type   ;

            virtual Deleter *    clone()
            {
                return new Deleter( *this )    ;
            }

            struct value_container
            {
                size_t      nKeyExpected    ;

                size_t      nSuccessItem    ;
                size_t      nFailedItem     ;

                value_container()
                    : nSuccessItem(0)
                    , nFailedItem(0)
                {}
            };

            struct erase_functor {
                value_container     m_cnt   ;

                void operator ()( pair_type& item )
                {
                    while ( true ) {
                        if ( item.second.bInitialized ) {
                            cds::lock::Auto< typename value_type::lock_type>    ac( item.second.m_access )  ;

                            if ( m_cnt.nKeyExpected == item.second.nKey && m_cnt.nKeyExpected * 8 == item.second.nData )
                                ++m_cnt.nSuccessItem  ;
                            else
                                ++m_cnt.nFailedItem   ;
                            item.second.nData++    ;
                            item.second.nKey = 0   ;
                            break           ;
                        }
                        else
                            cds::backoff::yield()() ;
                    }
                }
            };

        public:
            size_t  m_nDeleteSuccess    ;
            size_t  m_nDeleteFailed     ;

            size_t  m_nValueSuccess     ;
            size_t  m_nValueFailed      ;

        public:
            Deleter( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Deleter( Deleter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_func&  getTest()
            {
                return reinterpret_cast<Map_InsDel_func&>( m_Pool.m_Test )   ;
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map   ;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0 ;

                erase_functor   func    ;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            func.m_cnt.nKeyExpected = nItem ;
                            if ( rMap.erase( nItem, cds::ref(func) ))
                                ++m_nDeleteSuccess  ;
                            else
                                ++m_nDeleteFailed   ;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = c_nMapSize; nItem > 0; --nItem ) {
                            func.m_cnt.nKeyExpected = nItem - 1 ;
                            if ( rMap.erase( func.m_cnt.nKeyExpected, cds::ref(func) ))
                                ++m_nDeleteSuccess  ;
                            else
                                ++m_nDeleteFailed   ;
                        }
                    }
                }

                m_nValueSuccess = func.m_cnt.nSuccessItem   ;
                m_nValueFailed = func.m_cnt.nFailedItem     ;
            }
        };

    protected:

        template <class Map>
        void do_test( size_t nLoadFactor )
        {
            typedef Inserter<Map>       InserterThread  ;
            typedef Deleter<Map>        DeleterThread   ;
            typedef Ensurer<Map>        EnsurerThread   ;
            Map  testMap( c_nMapSize, nLoadFactor ) ;
            cds::OS::Timer    timer    ;

            CPPUNIT_MSG( "Load factor=" << nLoadFactor )   ;

            CppUnitMini::ThreadPool pool( *this )   ;
            pool.add( new InserterThread( pool, testMap ), c_nInsertThreadCount ) ;
            pool.add( new DeleterThread( pool, testMap ), c_nDeleteThreadCount ) ;
            pool.add( new EnsurerThread( pool, testMap ), c_nEnsureThreadCount ) ;
            pool.run()  ;
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() ) ;

            size_t nInsertSuccess = 0   ;
            size_t nInsertFailed = 0    ;
            size_t nDeleteSuccess = 0   ;
            size_t nDeleteFailed = 0    ;
            size_t nDelValueSuccess = 0 ;
            size_t nDelValueFailed = 0  ;
            size_t nEnsureFailed = 0    ;
            size_t nEnsureCreated = 0   ;
            size_t nEnsureModified = 0  ;
            size_t nEnsFuncCreated = 0  ;
            size_t nEnsFuncModified = 0 ;
            size_t nTestFunctorRef = 0  ;

            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                InserterThread * pThread = dynamic_cast<InserterThread *>( *it )   ;
                if ( pThread ) {
                    nInsertSuccess += pThread->m_nInsertSuccess ;
                    nInsertFailed += pThread->m_nInsertFailed   ;
                    nTestFunctorRef += pThread->m_nTestFunctorRef   ;
                }
                else {
                    DeleterThread * p = dynamic_cast<DeleterThread *>( *it ) ;
                    if ( p ) {
                        nDeleteSuccess += p->m_nDeleteSuccess   ;
                        nDeleteFailed += p->m_nDeleteFailed     ;
                        nDelValueSuccess += p->m_nValueSuccess  ;
                        nDelValueFailed += p->m_nValueFailed    ;
                    }
                    else {
                        EnsurerThread * pEns = static_cast<EnsurerThread *>( *it )    ;
                        nEnsureCreated += pEns->m_nEnsureCreated    ;
                        nEnsureModified += pEns->m_nEnsureExisted   ;
                        nEnsureFailed += pEns->m_nEnsureFailed      ;
                        nEnsFuncCreated += pEns->m_nFunctorCreated  ;
                        nEnsFuncModified += pEns->m_nFunctorModified;
                    }
                }
            }

            CPPUNIT_MSG( "    Totals: Ins succ=" << nInsertSuccess
                << " Del succ=" << nDeleteSuccess << "\n"
                << "          : Ins fail=" << nInsertFailed
                << " Del fail=" << nDeleteFailed << "\n"
                << "          : Ensure succ=" << (nEnsureCreated + nEnsureModified) << " fail=" << nEnsureFailed
                << " create=" << nEnsureCreated << " modify=" << nEnsureModified << "\n"
                << "          Map size=" << testMap.size()
                ) ;

            CPPUNIT_CHECK_EX( nDelValueFailed == 0, "Functor del failed=" << nDelValueFailed )  ;
            CPPUNIT_CHECK_EX( nDelValueSuccess == nDeleteSuccess,  "Delete success=" << nDeleteSuccess << " functor=" << nDelValueSuccess )    ;

            CPPUNIT_CHECK( nEnsureFailed == 0 )    ;

            CPPUNIT_CHECK_EX( nEnsureCreated == nEnsFuncCreated, "Ensure created=" << nEnsureCreated << " functor=" << nEnsFuncCreated )       ;
            CPPUNIT_CHECK_EX( nEnsureModified == nEnsFuncModified, "Ensure modified=" << nEnsureModified << " functor=" << nEnsFuncModified )  ;

            // nTestFunctorRef is call count of insert functor
            CPPUNIT_CHECK_EX( nTestFunctorRef == nInsertSuccess, "nInsertSuccess=" << nInsertSuccess << " functor nTestFunctorRef=" << nTestFunctorRef )  ;

            CPPUNIT_MSG( "  Clear map (single-threaded)..." ) ;
            timer.reset()   ;
            for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                testMap.erase( nItem )  ;
            }
            CPPUNIT_MSG( "   Duration=" << timer.duration() ) ;
            CPPUNIT_CHECK( testMap.empty() ) ;
        }

        template <class Map>
        void test()
        {
            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " ensure=" << c_nEnsureThreadCount
                << " pass count=" << c_nThreadPassCount
                << " map size=" << c_nMapSize
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                do_test<Map>( nLoadFactor )     ;
                if ( c_bPrintGCState )
                    print_gc_state()            ;
            }

        }

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            c_nInsertThreadCount = cfg.getULong("InsertThreadCount", 4 )  ;
            c_nDeleteThreadCount = cfg.getULong("DeleteThreadCount", 4 )  ;
            c_nEnsureThreadCount = cfg.getULong("EnsureThreadCount", 4 )  ;
            c_nThreadPassCount = cfg.getULong("ThreadPassCount", 4 )      ;
            c_nMapSize = cfg.getULong("MapSize", 1000000 )                ;
            c_nMaxLoadFactor = cfg.getULong("MaxLoadFactor", 8 )          ;
            c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true )      ;
        }

        TEST_MAP(MichaelMap_HP_cmp_stdAlloc)
        TEST_MAP(MichaelMap_HP_cmp_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_HP_cmp_michaelAlloc)
        TEST_MAP(MichaelMap_HP_less_stdAlloc)
        TEST_MAP(MichaelMap_HP_less_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_HP_less_michaelAlloc)
        TEST_MAP(MichaelMap_HRC_cmp_stdAlloc)
        TEST_MAP(MichaelMap_HRC_cmp_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_HRC_cmp_michaelAlloc)
        TEST_MAP(MichaelMap_HRC_less_stdAlloc)
        TEST_MAP(MichaelMap_HRC_less_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_HRC_less_michaelAlloc)
        TEST_MAP(MichaelMap_PTB_cmp_stdAlloc)
        TEST_MAP(MichaelMap_PTB_cmp_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_PTB_cmp_michaelAlloc)
        TEST_MAP(MichaelMap_PTB_less_stdAlloc)
        TEST_MAP(MichaelMap_PTB_less_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_PTB_less_michaelAlloc)

        TEST_MAP(MichaelMap_Lazy_HP_cmp_stdAlloc)
        TEST_MAP(MichaelMap_Lazy_HP_cmp_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_Lazy_HP_cmp_michaelAlloc)
        TEST_MAP(MichaelMap_Lazy_HP_less_stdAlloc)
        TEST_MAP(MichaelMap_Lazy_HP_less_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_Lazy_HP_less_michaelAlloc)
        TEST_MAP(MichaelMap_Lazy_HRC_cmp_stdAlloc)
        TEST_MAP(MichaelMap_Lazy_HRC_cmp_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_Lazy_HRC_cmp_michaelAlloc)
        TEST_MAP(MichaelMap_Lazy_HRC_less_stdAlloc)
        TEST_MAP(MichaelMap_Lazy_HRC_less_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_Lazy_HRC_less_michaelAlloc)
        TEST_MAP(MichaelMap_Lazy_PTB_cmp_stdAlloc)
        TEST_MAP(MichaelMap_Lazy_PTB_cmp_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_Lazy_PTB_cmp_michaelAlloc)
        TEST_MAP(MichaelMap_Lazy_PTB_less_stdAlloc)
        TEST_MAP(MichaelMap_Lazy_PTB_less_stdAlloc_seqcst)
        TEST_MAP(MichaelMap_Lazy_PTB_less_michaelAlloc)

        TEST_MAP(SplitList_Michael_HP_dyn_cmp)
        TEST_MAP(SplitList_Michael_HP_dyn_cmp_seqcst)
        TEST_MAP(SplitList_Michael_HP_st_cmp)
        TEST_MAP(SplitList_Michael_HP_st_cmp_seqcst)
        TEST_MAP(SplitList_Michael_HP_dyn_less)
        TEST_MAP(SplitList_Michael_HP_dyn_less_seqcst)
        TEST_MAP(SplitList_Michael_HP_st_less)
        TEST_MAP(SplitList_Michael_HP_st_less_seqcst)
        TEST_MAP(SplitList_Michael_HRC_dyn_cmp)
        TEST_MAP(SplitList_Michael_HRC_dyn_cmp_seqcst)
        TEST_MAP(SplitList_Michael_HRC_st_cmp)
        TEST_MAP(SplitList_Michael_HRC_st_cmp_seqcst)
        TEST_MAP(SplitList_Michael_HRC_dyn_less)
        TEST_MAP(SplitList_Michael_HRC_dyn_less_seqcst)
        TEST_MAP(SplitList_Michael_HRC_st_less)
        TEST_MAP(SplitList_Michael_HRC_st_less_seqcst)
        TEST_MAP(SplitList_Michael_PTB_dyn_cmp)
        TEST_MAP(SplitList_Michael_PTB_dyn_cmp_seqcst)
        TEST_MAP(SplitList_Michael_PTB_st_cmp)
        TEST_MAP(SplitList_Michael_PTB_st_cmp_seqcst)
        TEST_MAP(SplitList_Michael_PTB_dyn_less)
        TEST_MAP(SplitList_Michael_PTB_dyn_less_seqcst)
        TEST_MAP(SplitList_Michael_PTB_st_less)
        TEST_MAP(SplitList_Michael_PTB_st_less_seqcst)

        TEST_MAP(SplitList_Lazy_HP_dyn_cmp)
        TEST_MAP(SplitList_Lazy_HP_dyn_cmp_seqcst)
        TEST_MAP(SplitList_Lazy_HP_st_cmp)
        TEST_MAP(SplitList_Lazy_HP_st_cmp_seqcst)
        TEST_MAP(SplitList_Lazy_HP_dyn_less)
        TEST_MAP(SplitList_Lazy_HP_dyn_less_seqcst)
        TEST_MAP(SplitList_Lazy_HP_st_less)
        TEST_MAP(SplitList_Lazy_HP_st_less_seqcst)
        TEST_MAP(SplitList_Lazy_HRC_dyn_cmp)
        TEST_MAP(SplitList_Lazy_HRC_dyn_cmp_seqcst)
        TEST_MAP(SplitList_Lazy_HRC_st_cmp)
        TEST_MAP(SplitList_Lazy_HRC_st_cmp_seqcst)
        TEST_MAP(SplitList_Lazy_HRC_dyn_less)
        TEST_MAP(SplitList_Lazy_HRC_dyn_less_seqcst)
        TEST_MAP(SplitList_Lazy_HRC_st_less)
        TEST_MAP(SplitList_Lazy_HRC_st_less_seqcst)
        TEST_MAP(SplitList_Lazy_PTB_dyn_cmp)
        TEST_MAP(SplitList_Lazy_PTB_dyn_cmp_seqcst)
        TEST_MAP(SplitList_Lazy_PTB_st_cmp)
        TEST_MAP(SplitList_Lazy_PTB_st_cmp_seqcst)
        TEST_MAP(SplitList_Lazy_PTB_dyn_less)
        TEST_MAP(SplitList_Lazy_PTB_dyn_less_seqcst)
        TEST_MAP(SplitList_Lazy_PTB_st_less)
        TEST_MAP(SplitList_Lazy_PTB_st_less_seqcst)

        // Not implemented
        //TEST_MAP(StdMap_Spin)    ;
        //TEST_MAP(StdHashMap_Spin)    ;

        CPPUNIT_TEST_SUITE( Map_InsDel_func )
            CPPUNIT_TEST(MichaelMap_HP_cmp_stdAlloc)
            CPPUNIT_TEST(MichaelMap_HP_cmp_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_HP_cmp_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_HP_less_stdAlloc)
            CPPUNIT_TEST(MichaelMap_HP_less_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_HP_less_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_HRC_cmp_stdAlloc)
            CPPUNIT_TEST(MichaelMap_HRC_cmp_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_HRC_cmp_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_HRC_less_stdAlloc)
            CPPUNIT_TEST(MichaelMap_HRC_less_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_HRC_less_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_PTB_cmp_stdAlloc)
            CPPUNIT_TEST(MichaelMap_PTB_cmp_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_PTB_cmp_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_PTB_less_stdAlloc)
            CPPUNIT_TEST(MichaelMap_PTB_less_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_PTB_less_michaelAlloc)

            CPPUNIT_TEST(MichaelMap_Lazy_HP_cmp_stdAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_HP_cmp_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_Lazy_HP_cmp_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_HP_less_stdAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_HP_less_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_Lazy_HP_less_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_HRC_cmp_stdAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_HRC_cmp_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_Lazy_HRC_cmp_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_HRC_less_stdAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_HRC_less_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_Lazy_HRC_less_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_PTB_cmp_stdAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_PTB_cmp_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_Lazy_PTB_cmp_michaelAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_PTB_less_stdAlloc)
            CPPUNIT_TEST(MichaelMap_Lazy_PTB_less_stdAlloc_seqcst)
            CPPUNIT_TEST(MichaelMap_Lazy_PTB_less_michaelAlloc)

            CPPUNIT_TEST(SplitList_Michael_HP_dyn_cmp)
            CPPUNIT_TEST(SplitList_Michael_HP_dyn_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Michael_HP_st_cmp)
            CPPUNIT_TEST(SplitList_Michael_HP_st_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Michael_HP_dyn_less)
            CPPUNIT_TEST(SplitList_Michael_HP_dyn_less_seqcst)
            CPPUNIT_TEST(SplitList_Michael_HP_st_less)
            CPPUNIT_TEST(SplitList_Michael_HP_st_less_seqcst)
            CPPUNIT_TEST(SplitList_Michael_HRC_dyn_cmp)
            CPPUNIT_TEST(SplitList_Michael_HRC_dyn_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Michael_HRC_st_cmp)
            CPPUNIT_TEST(SplitList_Michael_HRC_st_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Michael_HRC_dyn_less)
            CPPUNIT_TEST(SplitList_Michael_HRC_dyn_less_seqcst)
            CPPUNIT_TEST(SplitList_Michael_HRC_st_less)
            CPPUNIT_TEST(SplitList_Michael_HRC_st_less_seqcst)
            CPPUNIT_TEST(SplitList_Michael_PTB_dyn_cmp)
            CPPUNIT_TEST(SplitList_Michael_PTB_dyn_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Michael_PTB_st_cmp)
            CPPUNIT_TEST(SplitList_Michael_PTB_st_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Michael_PTB_dyn_less)
            CPPUNIT_TEST(SplitList_Michael_PTB_dyn_less_seqcst)
            CPPUNIT_TEST(SplitList_Michael_PTB_st_less)
            CPPUNIT_TEST(SplitList_Michael_PTB_st_less_seqcst)

            CPPUNIT_TEST(SplitList_Lazy_HP_dyn_cmp)
            CPPUNIT_TEST(SplitList_Lazy_HP_dyn_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_HP_st_cmp)
            CPPUNIT_TEST(SplitList_Lazy_HP_st_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_HP_dyn_less)
            CPPUNIT_TEST(SplitList_Lazy_HP_dyn_less_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_HP_st_less)
            CPPUNIT_TEST(SplitList_Lazy_HP_st_less_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_HRC_dyn_cmp)
            CPPUNIT_TEST(SplitList_Lazy_HRC_dyn_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_HRC_st_cmp)
            CPPUNIT_TEST(SplitList_Lazy_HRC_st_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_HRC_dyn_less)
            CPPUNIT_TEST(SplitList_Lazy_HRC_dyn_less_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_HRC_st_less)
            CPPUNIT_TEST(SplitList_Lazy_HRC_st_less_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_PTB_dyn_cmp)
            CPPUNIT_TEST(SplitList_Lazy_PTB_dyn_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_PTB_st_cmp)
            CPPUNIT_TEST(SplitList_Lazy_PTB_st_cmp_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_PTB_dyn_less)
            CPPUNIT_TEST(SplitList_Lazy_PTB_dyn_less_seqcst)
            CPPUNIT_TEST(SplitList_Lazy_PTB_st_less)
            CPPUNIT_TEST(SplitList_Lazy_PTB_st_less_seqcst)

            // Not implemented
            //CPPUNIT_TEST( StdMap_Spin )
            //CPPUNIT_TEST( StdHashMap_Spin )
        CPPUNIT_TEST_SUITE_END();
    } ;

    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDel_func );
} // namespace map2
