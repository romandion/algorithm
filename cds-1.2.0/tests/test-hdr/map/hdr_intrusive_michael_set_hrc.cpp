/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "map/hdr_intrusive_set.h"
#include <cds/intrusive/michael_list_hrc.h>
#include <cds/intrusive/michael_set.h>

namespace map {

    void IntrusiveHashSetHdrTest::HRC_base_cmp()
    {
        typedef base_int_item< ci::michael_list::node<cds::gc::HRC> > item ;
        typedef ci::MichaelList< cds::gc::HRC
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type    ;

        typedef ci::MichaelHashSet< cds::gc::HRC, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set ;

        test_int<set>()    ;
    }

    void IntrusiveHashSetHdrTest::HRC_base_less()
    {
        typedef base_int_item< ci::michael_list::node<cds::gc::HRC> > item ;
        typedef ci::MichaelList< cds::gc::HRC
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type    ;

        typedef ci::MichaelHashSet< cds::gc::HRC, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set ;

        test_int<set>()    ;
    }

    void IntrusiveHashSetHdrTest::HRC_base_cmpmix()
    {
        typedef base_int_item< ci::michael_list::node<cds::gc::HRC> > item ;
        typedef ci::MichaelList< cds::gc::HRC
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type    ;

        typedef ci::MichaelHashSet< cds::gc::HRC, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
            >::type
        > set ;

        test_int<set>()    ;
    }

} // namespace map
