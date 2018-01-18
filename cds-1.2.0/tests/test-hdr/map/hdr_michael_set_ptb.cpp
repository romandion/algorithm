/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map/hdr_set.h"
#include <cds/container/michael_list_ptb.h>
#include <cds/container/michael_set.h>

namespace map {

    namespace {
        struct PTB_cmp_traits: public cc::michael_list::type_traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare ;
        };

        struct PTB_less_traits: public cc::michael_list::type_traits
        {
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less ;
        };

        struct PTB_cmpmix_traits: public cc::michael_list::type_traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare ;
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less ;
        };
    }

    void HashSetHdrTest::PTB_cmp()
    {
        typedef cc::MichaelList< cds::gc::PTB, item, PTB_cmp_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::PTB, list, set_traits > set     ;
        test_int< set >()  ;

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::PTB, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set   ;
        test_int< opt_set >()  ;
    }

    void HashSetHdrTest::PTB_less()
    {
        typedef cc::MichaelList< cds::gc::PTB, item, PTB_less_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::PTB, list, set_traits > set     ;
        test_int< set >()  ;

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::PTB, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set   ;
        test_int< opt_set >()  ;
    }

    void HashSetHdrTest::PTB_cmpmix()
    {
        typedef cc::MichaelList< cds::gc::PTB, item, PTB_cmpmix_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::PTB, list, set_traits > set     ;
        test_int< set >()  ;

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::PTB, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set   ;
        test_int< opt_set >()  ;
    }


} // namespace map
