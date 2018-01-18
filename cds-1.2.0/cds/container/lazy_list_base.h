/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#ifndef __CDS_CONTAINER_LAZY_LIST_BASE_H
#define __CDS_CONTAINER_LAZY_LIST_BASE_H

#include <cds/container/base.h>
#include <cds/intrusive/lazy_list_base.h>

namespace cds { namespace container {

    /// LazyList ordered list related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace lazy_list {
        /// Lazy list default type traits
        /**
            Either \p compare or \p less or both must be specified.
        */
        struct type_traits
        {
            /// allocator used to allocate new node
            typedef CDS_DEFAULT_ALLOCATOR   allocator       ;

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare         ;

            /// specifies binary predicate used for key comparision.
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less            ;

            /// Lock type used to lock modifying items
            /**
                Default is cds::lock::Spin
            */
            typedef cds::lock::Spin                 lock_type       ;

            /// back-off strategy used
            /**
                If the option is not specified, the cds::backoff::Default is used.
            */
            typedef cds::backoff::Default           back_off        ;

            /// Item counter
            /**
                The type for item counting feature.
                Default is no item counter (\ref atomicity::empty_item_counter)
            */
            typedef atomicity::empty_item_counter     item_counter    ;

            /// Link fields checking feature
            /**
                Default is \ref intrusive::opt::debug_check_link
            */
            static const opt::link_check_type link_checker = opt::debug_check_link  ;

            /// C++ memory ordering model
            /**
                List of available memory ordering see opt::memory_model
            */
            typedef opt::v::relaxed_ordering        memory_model    ;


            //@cond
            // LazyKVList: supporting for split-ordered list
            // key accessor (opt::none = internal key type is equal to user key type)
            typedef opt::none                       key_accessor    ;

            // for internal use only!!!
            typedef opt::none                       boundary_node_type  ;

            //@endcond
        };

        /// Metafunction converting option list to traits
        /**
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>

            See \ref LazyList, \ref type_traits, \ref cds::opt::make_options.
        */
        template <CDS_DECL_OPTIONS10>
        struct make_traits {
            typedef typename cds::opt::make_options< type_traits, CDS_OPTIONS10>::type type  ;   ///< Result of metafunction
        };


    } // namespace lazy_list

    // Forward declarations
    template <typename GC, typename T, typename Traits=lazy_list::type_traits>
    class LazyList ;

    template <typename GC, typename Key, typename Value, typename Traits=lazy_list::type_traits>
    class LazyKVList ;

    // Tag for selecting lazy list implementation
    /**
        This struct is empty and it is used only as a tag for selecting LazyList
        as ordered list implementation in declaration of some classes.

        See split_list::type_traits::ordered_list as an example.
    */
    struct lazy_list_tag
    {};


}}  // namespace cds::container


#endif  // #ifndef __CDS_CONTAINER_LAZY_LIST_BASE_H