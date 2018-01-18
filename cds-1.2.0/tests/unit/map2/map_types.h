/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#ifndef _CDSUNIT_MAP2_MAP_TYPES_H
#define _CDSUNIT_MAP2_MAP_TYPES_H

#include <cds/container/michael_kvlist_hp.h>
#include <cds/container/michael_kvlist_hrc.h>
#include <cds/container/michael_kvlist_ptb.h>
#include <cds/container/lazy_kvlist_hp.h>
#include <cds/container/lazy_kvlist_hrc.h>
#include <cds/container/lazy_kvlist_ptb.h>
#include <cds/container/michael_map.h>
#include <cds/container/split_list_map.h>

#include <cds/lock/spinlock.h>

#include "lock/nolock.h"
#include "map2/std_map.h"
#include "map2/std_hash_map.h"
#include "michael_alloc.h"


namespace map2 {
    namespace cc = cds::container   ;
    namespace co = cds::opt         ;

    template <typename Key>
    struct cmp {
        int operator ()(Key const& k1, Key const& k2) const
        {
            if ( std::less<Key>( k1, k2 ) )
                return -1   ;
            return std::less<Key>( k2, k1 ) ? 1 : 0  ;
        }
    };

#define CDSUNIT_INT_COMPARE(t)  template <> struct cmp<t> { int operator()( t k1, t k2 ){ return (int)(k1 - k2); } }
    CDSUNIT_INT_COMPARE(char)   ;
    CDSUNIT_INT_COMPARE(unsigned char)  ;
    CDSUNIT_INT_COMPARE(int)  ;
    CDSUNIT_INT_COMPARE(unsigned int)  ;
    CDSUNIT_INT_COMPARE(long)  ;
    CDSUNIT_INT_COMPARE(unsigned long)  ;
    CDSUNIT_INT_COMPARE(long long)  ;
    CDSUNIT_INT_COMPARE(unsigned long long)  ;
#undef CDSUNIT_INT_COMPARE

    template <>
    struct cmp<std::string>
    {
        int operator()(std::string const& s1, std::string const& s2)
        {
            return s1.compare( s2 ) ;
        }
        int operator()(std::string const& s1, char const * s2)
        {
            return s1.compare( s2 ) ;
        }
        int operator()(char const * s1, std::string const& s2)
        {
            return -s2.compare( s1 ) ;
        }
    };


    template <typename Key, typename Value>
    struct MapTypes {
        typedef co::v::hash<Key>    hash    ;
        typedef std::less<Key>      less    ;
        typedef cmp<Key>            compare ;

        // ***************************************************************************
        // MichaelKVList

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_HP_cmp_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HP_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HP_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_HP_less_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HP_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HP_less_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_HRC_cmp_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HRC_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HRC_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_HRC_less_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HRC_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HRC_less_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_PTB_cmp_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_PTB_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_PTB_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_PTB_less_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_PTB_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_PTB_less_michaelAlloc ;

        // ***************************************************************************
        // MichaelHashMap based on MichaelKVList

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HP_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HP_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_HP_cmp_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HP_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HP_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_HP_less_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HRC_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HRC_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_HRC_cmp_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HRC_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HRC_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_HRC_less_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_PTB_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_PTB_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_PTB_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_PTB_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_PTB_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_PTB_less_michaelAlloc  ;

        // ***************************************************************************
        // LazyKVList

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_HP_cmp_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HP_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HP_cmp_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_HP_less_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HP_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HP_less_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_HRC_cmp_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HRC_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HRC_cmp_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_HRC_less_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HRC_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HRC_less_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_PTB_cmp_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_PTB_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_PTB_cmp_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_PTB_less_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_PTB_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_PTB_less_michaelAlloc ;


        // ***************************************************************************
        // MichaelHashMap based on LazyKVList

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HP_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HP_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_HP_cmp_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HP_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HP_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_HP_less_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HRC_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HRC_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_HRC_cmp_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HRC_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HRC_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_HRC_less_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_PTB_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_PTB_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_PTB_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_PTB_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_PTB_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_PTB_less_michaelAlloc  ;


        // ***************************************************************************
        // SplitListMap based on MichaelKVList

        // HP
        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_cmp ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_cmp_seqcst ;

        //HP + less
        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_less ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_less ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_less_seqcst ;

        // HRC
        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_st_cmp ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_st_cmp_seqcst ;

        // HRC + less
        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_dyn_less ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_st_less ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_st_less_seqcst ;

        // PTB
        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_st_cmp ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_st_cmp_seqcst ;

        // PTB + less
        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_dyn_less ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_st_less ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_st_less_seqcst ;

        // ***************************************************************************
        // SplitListMap based on LazyKVList

        // HP
        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_cmp ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_cmp_seqcst ;


        // HP + less
        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_less ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_less ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_less_seqcst ;

        // HRC
        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_st_cmp ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_st_cmp_seqcst ;

        // HRC + less
        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_dyn_less ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_st_less ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_st_less_seqcst ;


        // PTB
        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_st_cmp ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_st_cmp_seqcst ;

        // PTB + less
        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_dyn_less ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_st_less ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_st_less_seqcst ;


        // ***************************************************************************
        // Standard implementations

        typedef StdMap< Key, Value, cds::SpinLock >             StdMap_Spin             ;
        typedef StdMap< Key, Value, lock::NoLock>               StdMap_NoLock           ;

        typedef StdHashMap< Key, Value, cds::SpinLock >         StdHashMap_Spin         ;
        typedef StdHashMap< Key, Value, lock::NoLock >          StdHashMap_NoLock       ;

    };

}   // namespace map2

#endif // ifndef _CDSUNIT_MAP2_MAP_TYPES_H
