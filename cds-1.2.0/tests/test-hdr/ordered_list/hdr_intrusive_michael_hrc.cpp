/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include "ordered_list/hdr_intrusive_michael.h"
#include <cds/intrusive/michael_list_hrc.h>

namespace ordlist {

    void IntrusiveMichaelListHeaderTest::HRC_base_cmp()
    {
        typedef base_int_item< cds::gc::HRC > item ;
        typedef ci::MichaelList< cds::gc::HRC
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list    ;
        test_int<list>()    ;
    }
    void IntrusiveMichaelListHeaderTest::HRC_base_less()
    {
        typedef base_int_item< cds::gc::HRC > item ;
        typedef ci::MichaelList< cds::gc::HRC
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list    ;
        test_int<list>()    ;
    }
    void IntrusiveMichaelListHeaderTest::HRC_base_cmpmix()
    {
        typedef base_int_item< cds::gc::HRC > item ;
        typedef ci::MichaelList< cds::gc::HRC
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list    ;
        test_int<list>()    ;
    }
    void IntrusiveMichaelListHeaderTest::HRC_base_ic()
    {
        typedef base_int_item< cds::gc::HRC > item ;
        typedef ci::MichaelList< cds::gc::HRC
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list    ;
        test_int<list>()    ;
    }

}
