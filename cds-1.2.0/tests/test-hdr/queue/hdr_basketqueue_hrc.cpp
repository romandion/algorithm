/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include <cds/container/basket_queue.h>
#include <cds/gc/hrc.h>

#include "queue/queue_test_header.h"

namespace queue {

    void Queue_TestHeader::BasketQueue_HRC()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
            >
        >()    ;
    }

    void Queue_TestHeader::BasketQueue_HRC_Counted()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        >() ;
    }

    void Queue_TestHeader::BasketQueue_HRC_relax()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >
        >()    ;
    }

    void Queue_TestHeader::BasketQueue_HRC_Counted_relax()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >
        >() ;
    }

    void Queue_TestHeader::BasketQueue_HRC_seqcst()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >
        >()    ;
    }

    void Queue_TestHeader::BasketQueue_HRC_Counted_seqcst()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >
        >() ;
    }

    void Queue_TestHeader::BasketQueue_HRC_relax_align()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 16 >
            >
        >()    ;
    }

    void Queue_TestHeader::BasketQueue_HRC_Counted_relax_align()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 32 >
            >
        >() ;
    }

    void Queue_TestHeader::BasketQueue_HRC_seqcst_align()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::no_special_alignment >
            >
        >()    ;
    }

    void Queue_TestHeader::BasketQueue_HRC_Counted_seqcst_align()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::cache_line_alignment >
            >
        >() ;
    }

}   // namespace queue
