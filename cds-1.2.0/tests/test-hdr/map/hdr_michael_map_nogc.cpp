/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map/hdr_map.h"
#include <cds/container/michael_kvlist_nogc.h>
#include <cds/container/michael_map_nogc.h>

namespace map {

    namespace {
        struct nogc_cmp_traits: public cc::michael_list::type_traits
        {
            typedef HashMapHdrTest::cmp   compare ;
        };

        struct nogc_less_traits: public cc::michael_list::type_traits
        {
            typedef HashMapHdrTest::less  less ;
        };

        struct nogc_cmpmix_traits: public cc::michael_list::type_traits
        {
            typedef HashMapHdrTest::cmp   compare ;
            typedef HashMapHdrTest::less  less ;
        };
    }

    void HashMapHdrTest::nogc_cmp()
    {
        typedef cc::MichaelKVList< cds::gc::nogc, int, HashMapHdrTest::value_type, nogc_cmp_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list, map_traits > map     ;
        test_int_nogc< map >()  ;

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map   ;
        test_int_nogc< opt_map >()  ;
    }

    void HashMapHdrTest::nogc_less()
    {
        typedef cc::MichaelKVList< cds::gc::nogc, int, HashMapHdrTest::value_type, nogc_less_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list, map_traits > map     ;
        test_int_nogc< map >()  ;

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map   ;
        test_int_nogc< opt_map >()  ;
    }

    void HashMapHdrTest::nogc_cmpmix()
    {
        typedef cc::MichaelKVList< cds::gc::nogc, int, HashMapHdrTest::value_type, nogc_cmpmix_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list, map_traits > map     ;
        test_int_nogc< map >()  ;

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map   ;
        test_int_nogc< opt_map >()  ;
    }

}   // namespace map
