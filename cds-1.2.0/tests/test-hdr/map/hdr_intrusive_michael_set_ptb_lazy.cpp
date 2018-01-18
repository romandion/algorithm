/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map/hdr_intrusive_set.h"
#include <cds/intrusive/lazy_list_ptb.h>
#include <cds/intrusive/michael_set.h>

namespace map {

    void IntrusiveHashSetHdrTest::PTB_base_cmp_lazy()
    {
        typedef base_int_item< ci::lazy_list::node<cds::gc::PTB> > item ;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::PTB> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type    ;

        typedef ci::MichaelHashSet< cds::gc::PTB, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set ;

        test_int<set>()    ;
    }

    void IntrusiveHashSetHdrTest::PTB_base_less_lazy()
    {
        typedef base_int_item< ci::lazy_list::node<cds::gc::PTB> > item ;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::PTB> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type    ;

        typedef ci::MichaelHashSet< cds::gc::PTB, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set ;

        test_int<set>()    ;
    }

    void IntrusiveHashSetHdrTest::PTB_base_cmpmix_lazy()
    {
        typedef base_int_item< ci::lazy_list::node<cds::gc::PTB> > item ;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::PTB> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type    ;

        typedef ci::MichaelHashSet< cds::gc::PTB, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
            >::type
        > set ;

        test_int<set>()    ;
    }

    void IntrusiveHashSetHdrTest::PTB_member_cmp_lazy()
    {
        typedef member_int_item< ci::lazy_list::node<cds::gc::PTB> > item ;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::PTB>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type    ;

        typedef ci::MichaelHashSet< cds::gc::PTB, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set ;

        test_int<set>()    ;
    }

    void IntrusiveHashSetHdrTest::PTB_member_less_lazy()
    {
        typedef member_int_item< ci::lazy_list::node<cds::gc::PTB> > item ;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::PTB>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type    ;

        typedef ci::MichaelHashSet< cds::gc::PTB, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set ;

        test_int<set>()    ;
    }

    void IntrusiveHashSetHdrTest::PTB_member_cmpmix_lazy()
    {
        typedef member_int_item< ci::lazy_list::node<cds::gc::PTB> > item ;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::PTB>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type    ;

        typedef ci::MichaelHashSet< cds::gc::PTB, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
            >::type
        > set ;

        test_int<set>()    ;
    }


} // namespace map