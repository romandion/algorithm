/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map/hdr_map.h"
#include <cds/container/michael_kvlist_ptb.h>
#include <cds/container/michael_map.h>

namespace map {

    namespace {
        struct PTB_cmp_traits: public cc::michael_list::type_traits
        {
            typedef HashMapHdrTest::cmp   compare ;
        };

        struct PTB_less_traits: public cc::michael_list::type_traits
        {
            typedef HashMapHdrTest::less  less ;
        };

        struct PTB_cmpmix_traits: public cc::michael_list::type_traits
        {
            typedef HashMapHdrTest::cmp   compare ;
            typedef HashMapHdrTest::less  less ;
        };
    }

    void HashMapHdrTest::PTB_cmp()
    {
        typedef cc::MichaelKVList< cds::gc::PTB, int, HashMapHdrTest::value_type, PTB_cmp_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list, map_traits > map     ;
        test_int< map >()  ;

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map   ;
        test_int< opt_map >()  ;
    }

    void HashMapHdrTest::PTB_less()
    {
        typedef cc::MichaelKVList< cds::gc::PTB, int, HashMapHdrTest::value_type, PTB_less_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list, map_traits > map     ;
        test_int< map >()  ;

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map   ;
        test_int< opt_map >()  ;
    }

    void HashMapHdrTest::PTB_cmpmix()
    {
        typedef cc::MichaelKVList< cds::gc::PTB, int, HashMapHdrTest::value_type, PTB_cmpmix_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list, map_traits > map     ;
        test_int< map >()  ;

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map   ;
        test_int< opt_map >()  ;
    }


} // namespace map

