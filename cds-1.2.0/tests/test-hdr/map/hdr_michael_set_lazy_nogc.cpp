/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map/hdr_set.h"
#include <cds/container/lazy_list_nogc.h>
#include <cds/container/michael_set_nogc.h>

namespace map {

    namespace {
        struct nogc_cmp_traits: public cc::lazy_list::type_traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare ;
        };

        struct nogc_less_traits: public cc::lazy_list::type_traits
        {
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less ;
        };

        struct nogc_cmpmix_traits: public cc::lazy_list::type_traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare ;
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less ;
        };
    }

    void HashSetHdrTest::Lazy_nogc_cmp()
    {
        typedef cc::LazyList< cds::gc::nogc, item, nogc_cmp_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list, set_traits > set     ;
        test_int_nogc< set >()  ;

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set   ;
        test_int_nogc< opt_set >()  ;
    }

    void HashSetHdrTest::Lazy_nogc_less()
    {
        typedef cc::LazyList< cds::gc::nogc, item, nogc_less_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list, set_traits > set     ;
        test_int_nogc< set >()  ;

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set   ;
        test_int_nogc< opt_set >()  ;
    }

    void HashSetHdrTest::Lazy_nogc_cmpmix()
    {
        typedef cc::LazyList< cds::gc::nogc, item, nogc_cmpmix_traits > list   ;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list, set_traits > set     ;
        test_int_nogc< set >()  ;

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set   ;
        test_int_nogc< opt_set >()  ;
    }


} // namespace map
