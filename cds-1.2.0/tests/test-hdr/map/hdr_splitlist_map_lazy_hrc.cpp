/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map/hdr_map.h"
#include <cds/container/lazy_list_hrc.h>
#include <cds/container/split_list_map.h>

namespace map {

    namespace {
        struct HRC_cmp_traits: public cc::split_list::type_traits
        {
            typedef cc::lazy_list_tag                   ordered_list    ;
            typedef HashMapHdrTest::hash_int            hash            ;
            typedef HashMapHdrTest::simple_item_counter item_counter    ;
            typedef cc::opt::v::relaxed_ordering        memory_model    ;
            enum { dynamic_bucket_table = false };

            struct ordered_list_traits: public cc::lazy_list::type_traits
            {
                typedef HashMapHdrTest::cmp   compare ;
            } ;
        };

        struct HRC_less_traits: public cc::split_list::type_traits
        {
            typedef cc::lazy_list_tag                   ordered_list    ;
            typedef HashMapHdrTest::hash_int            hash            ;
            typedef HashMapHdrTest::simple_item_counter item_counter    ;
            typedef cc::opt::v::sequential_consistent                      memory_model    ;
            enum { dynamic_bucket_table = false };

            struct ordered_list_traits: public cc::lazy_list::type_traits
            {
                typedef HashMapHdrTest::less   less ;
            };
        };

        struct HRC_cmpmix_traits: public cc::split_list::type_traits
        {
            typedef cc::lazy_list_tag                   ordered_list    ;
            typedef HashMapHdrTest::hash_int            hash            ;
            typedef HashMapHdrTest::simple_item_counter item_counter    ;

            struct ordered_list_traits: public cc::lazy_list::type_traits
            {
                typedef HashMapHdrTest::cmp   compare ;
                typedef std::less<HashMapHdrTest::key_type>     less ;
            };
        };
    }

    void HashMapHdrTest::Split_Lazy_HRC_cmp()
    {
        // traits-based version
        typedef cc::SplitListMap< cds::gc::HRC, key_type, value_type, HRC_cmp_traits > map_type     ;
        test_int< map_type >()  ;

        // option-based version
        typedef cc::SplitListMap< cds::gc::HRC,
            key_type,
            value_type,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::opt::memory_model< cc::opt::v::relaxed_ordering >
                ,cc::split_list::dynamic_bucket_table< true >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                        cc::opt::compare< cmp >
                    >::type
                >
            >::type
        > opt_map   ;
        test_int< opt_map >()  ;
    }

    void HashMapHdrTest::Split_Lazy_HRC_less()
    {
        // traits-based version
        typedef cc::SplitListMap< cds::gc::HRC, key_type, value_type, HRC_less_traits > map_type     ;
        test_int< map_type >()  ;

        // option-based version
        typedef cc::SplitListMap< cds::gc::HRC,
            key_type,
            value_type,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::opt::memory_model< cc::opt::v::relaxed_ordering >
                ,cc::split_list::dynamic_bucket_table< false >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                        cc::opt::less< less >
                    >::type
                >
            >::type
        > opt_map   ;
        test_int< opt_map >()  ;
    }

    void HashMapHdrTest::Split_Lazy_HRC_cmpmix()
    {
        // traits-based version
        typedef cc::SplitListMap< cds::gc::HRC, key_type, value_type, HRC_cmpmix_traits > map_type   ;
        test_int< map_type >()  ;

        // option-based version
        typedef cc::SplitListMap< cds::gc::HRC,
            key_type,
            value_type,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                    cc::opt::less< std::less<key_type> >
                        ,cc::opt::compare< cmp >
                    >::type
                >
            >::type
        > opt_map   ;
        test_int< opt_map >()  ;
    }


} // namespace map

