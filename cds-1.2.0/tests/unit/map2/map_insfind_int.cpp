/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map2/map_types.h"
#include "cppunit/thread.h"

#include <cds/os/topology.h>

namespace map2 {

#    define TEST_MAP(X)    void X() { test<MapTypes<key_type, value_type>::X >()    ; }

    namespace {
        static size_t  c_nMapSize = 1000000    ;  // map size
        static size_t  c_nThreadCount = 4      ;  // count of insertion thread
        static size_t  c_nMaxLoadFactor = 8    ;  // maximum load factor
        static bool    c_bPrintGCState = true  ;
    }

    class Map_InsFind_int: public CppUnitMini::TestCase
    {
        typedef size_t  key_type    ;
        typedef size_t  value_type  ;

        template <class MAP>
        class Inserter: public CppUnitMini::TestThread
        {
            MAP&     m_Map      ;

            virtual Inserter *    clone()
            {
                return new Inserter( *this )    ;
            }
        public:
            size_t  m_nInsertSuccess    ;
            size_t  m_nInsertFailed     ;
            size_t  m_nFindSuccess      ;
            size_t  m_nFindFail         ;

        public:
            Inserter( CppUnitMini::ThreadPool& pool, MAP& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Inserter( Inserter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsFind_int&  getTest()
            {
                return reinterpret_cast<Map_InsFind_int&>( m_Pool.m_Test )   ;
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                MAP& rMap = m_Map   ;

                m_nInsertSuccess =
                    m_nInsertFailed =
                    m_nFindSuccess =
                    m_nFindFail = 0 ;

                size_t nInc = c_nThreadCount    ;
                for ( size_t nItem = m_nThreadNo; rMap.size() < c_nMapSize; nItem += nInc ) {
                    if ( rMap.insert( nItem, nItem * 8 ) )
                        ++m_nInsertSuccess  ;
                    else
                        ++m_nInsertFailed   ;

                    for ( size_t nFind = m_nThreadNo; nFind <= nItem; nFind += nInc ) {
                        if ( rMap.find( nFind) )
                            ++m_nFindSuccess    ;
                        else
                            ++m_nFindFail       ;
                    }
                }
            }
        };

    protected:

        template <class MAP>
        void do_test( size_t nLoadFactor )
        {
            typedef Inserter<MAP>       InserterThread  ;
            MAP  testMap( c_nMapSize, nLoadFactor ) ;
            cds::OS::Timer    timer    ;

            CPPUNIT_MSG( "Load factor=" << nLoadFactor )   ;

            CppUnitMini::ThreadPool pool( *this )   ;
            pool.add( new InserterThread( pool, testMap ), c_nThreadCount ) ;
            pool.run()  ;
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() ) ;

            size_t nInsertSuccess = 0   ;
            size_t nInsertFailed = 0    ;
            size_t nFindSuccess = 0     ;
            size_t nFindFailed = 0      ;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                InserterThread * pThread = static_cast<InserterThread *>( *it )   ;

                nInsertSuccess += pThread->m_nInsertSuccess ;
                nInsertFailed += pThread->m_nInsertFailed   ;
                nFindSuccess += pThread->m_nFindSuccess     ;
                nFindFailed += pThread->m_nFindFail         ;
            }

            CPPUNIT_MSG( "    Totals: Ins succ=" << nInsertSuccess << " fail=" << nInsertFailed << "\n"
                      << "           Find succ=" << nFindSuccess << " fail=" << nFindFailed
            ) ;

            CPPUNIT_ASSERT( nInsertFailed == 0 )    ;
            CPPUNIT_ASSERT( nFindFailed == 0 )      ;
        }

        template <class MAP>
        void test()
        {
            CPPUNIT_MSG( "Thread count: " << c_nThreadCount
                << " map size=" << c_nMapSize
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                do_test<MAP>( nLoadFactor )     ;
                if ( c_bPrintGCState )
                    print_gc_state()            ;
            }

        }

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            c_nThreadCount = cfg.getULong("ThreadCount", 0 )            ;
            c_nMapSize = cfg.getULong("MapSize", 10000 )                ;
            c_nMaxLoadFactor = cfg.getULong("MaxLoadFactor", 8 )        ;
            c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true )    ;
            if ( c_nThreadCount == 0 )
                c_nThreadCount = cds::OS::topology::processor_count()   ;
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

        TEST_MAP(StdMap_Spin)    ;
        TEST_MAP(StdHashMap_Spin)    ;

        CPPUNIT_TEST_SUITE( Map_InsFind_int )
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

            CPPUNIT_TEST( StdMap_Spin )
            CPPUNIT_TEST( StdHashMap_Spin )
        CPPUNIT_TEST_SUITE_END();
    } ;

    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsFind_int );
} // namespace map2
