/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map/hdr_set.h"
#include <cds/container/lazy_list_hp.h>
#include <cds/container/michael_set.h>

namespace map {

    namespace {
        struct HP_cmp_traits: public cc::lazy_list::type_traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare ;
        };

        struct HP_less_traits: public cc::lazy_list::type_traits
        {
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less ;
        };

        struct HP_cmpmix_traits: public cc::lazy_list::type_traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare ;
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less ;
        };
    }

    void HashSetHdrTest::Lazy_HP_cmp()
    {
        typedef cc::LazyList< cds::gc::HP, item, HP_cmp_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list, set_traits > set     ;
        test_int< set >()  ;

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set   ;
        test_int< opt_set >()  ;
    }

    void HashSetHdrTest::Lazy_HP_less()
    {
        typedef cc::LazyList< cds::gc::HP, item, HP_less_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list, set_traits > set     ;
        test_int< set >()  ;

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set   ;
        test_int< opt_set >()  ;
    }

    void HashSetHdrTest::Lazy_HP_cmpmix()
    {
        typedef cc::LazyList< cds::gc::HP, item, HP_cmpmix_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list, set_traits > set     ;
        test_int< set >()  ;

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set   ;
        test_int< opt_set >()  ;
    }


} // namespace map

