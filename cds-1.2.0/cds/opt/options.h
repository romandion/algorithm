/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#ifndef __CDS_OPT_OPTIONS_H
#define __CDS_OPT_OPTIONS_H

/*
    Framework to define template options

    Editions:
        2011.01.23 khizmax  Created
*/

#include <cds/details/aligned_type.h>
#include <cds/user_setup/cache_line.h>
#include <cds/cxx11_atomic.h>

namespace cds {

/// Framework to define template options
/**
    There are two kind of options:
    - \p type-option - option that determines a data type. The template argument \p Type of the option is a type.
    - \p value-option - option that determines a value. The template argument \p Value of the option is a value.
*/
namespace opt {

    /// Predefined options value (generally, for the options that determine the data types)
    namespace v {}

    /// Type indicates that an option is not specified and the default one should be used
    struct none
    {
        //@cond
        template <class Base> struct pack: public Base
        {};
        //@endcond
    };

    /// Metafunction for selecting default option value
    /**
        Template parameters:
        - \p Option - option value
        - \p Default - default option value
        - \p Value - option value if \p Option is not opt::none

        If \p Option is opt::none, the metafunction result is \p Default, otherwise
        the result is \p Value.

        Examples:
        \code
        // default_spin is cds::lock::Spin
        typedef typename cds::opt::select_default< cds::opt::none, cds::lock::Spin >::type  default_spin    ;

        // spin_32bit is cds::lock::Spin32
        typedef typename cds::opt::select_default< cds::lock::Spin32, cds::lock::Spin >::type  spin_32bit   ;
        \endcode
    */
    template <typename Option, typename Default, typename Value = Option>
    struct select_default
    {
        typedef Value type ;   ///< metafunction result
    };
    //@cond
    template <typename Default>
    struct select_default< none, Default >
    {
        typedef Default type    ;
    };
    //@endcond


    /// [type-option] Option setter specifies a tag
    /**
        Suppose, you have a struct
        \code
        struct Feature
        {  .... };
        \endcode
        and you want that your class \p X would be derived from several \p Feature:
        \code
            class X: public Feature, public Feature
            { .... };
        \endcode

        How can you distinguish one \p Feature from another?
        You may use a tag option:
        \code
            template <typename Tag>
            struct Feature
            { .... };

            class tag_a;
            class tag_b;
            class X: public Feature< tag_a >, public Feature< tag_b >
            { .... };
        \endcode
        Now you can distinguish one \p Feature from another:
        \code
            X x ;
            Feature<tag_a>& fa = static_cast< Feature<tag_a> >( x ) ;
            Feature<tag_b>& fb = static_cast< Feature<tag_b> >( x ) ;
        \endcode

        \p tag option setter allows you to do things like this for an option-centric approach:
        \code
        template <typename ...Options>
        struct Feature
        { .... };

        class tag_a;
        class tag_b;
        class X: public Feature< tag<tag_a> >, public Feature< tag<tag_b> >
        { .... };
        \endcode

        This option setter is widely used in cds::intrusive containers to distinguish
        between different intrusive part of container's node.

        An incomplete type can serve as a \p Tag.
    */
    template <typename Tag>
    struct tag {
        //@cond
        template<class Base> struct pack: public Base
        {
            typedef Tag tag ;
        };
        //@endcond
    };

    /// [type-option] Option setter specifies lock class
    /**
        Specification of the \p Type class is:
        \code
        struct Lock {
            void lock() ;
            void unlock() ;
        };
        \endcode
    */
    template <typename Type>
    struct lock_type {
        //@cond
        template<class Base> struct pack: public Base
        {
            typedef Type lock_type ;
        };
        //@endcond
    };

    /// [type-option] Back-off strategy option setter
    /**
        Back-off strategy used in some algorithm.
        See cds::backoff namespace for back-off explanation and supported interface.
    */
    template <typename Type>
    struct back_off {
        //@cond
        template <class Base> struct pack: public Base
        {
            typedef Type back_off   ;
        };
        //@endcond
    };

    /// [type-option] Option setter for garbage collecting schema used
    /**
        Possible values of \p GC template parameter are:
        - cds::gc::HP - Hazard Pointer garbage collector
        - cds::gc::HRC - Gidenstam's garbage collector
        - cds::gc::PTB - Pass-the-Buck garbage collector
        - cds::gc::none::GC - No garbage collector (not supported for some containers)
    */
    template <typename GC>
    struct gc {
        //@cond
        template <class Base> struct pack: public Base
        {
            typedef GC gc   ;
        };
        //@endcond
    };

    /// [type-option] Option setter for an allocator
    /**
        \p Type is allocator with \p std::allocator interface. Default is value of macro CDS_DEFAULT_ALLOCATOR
        that, in turn, is \p std::allocator.

        The \p libcds containers actively use rebinding to convert an allocator of one type to another one. Thus,
        you may specify any valid type as std::allocator's template parameter.
    */
    template <typename Type>
    struct allocator {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type allocator ;
        };
        //@endcond
    };

    /// [type-option] Option setter for item counting
    /**
        Some data structure (for example, queues) has additional feature for item counting.
        This option allows to set up appropriate item counting policy for that data structure.

        Predefined option \p Type:
        - atomicity::empty_item_counter - no item counting performed. It is default policy for many
            containers
        - atomicity::item_counter - the class that provides atomically item counting

        You may provide other implementation of atomicity::item_counter interface for your needs.

        Note, the item counting in lock-free containers cannot be exact; for example, if
        item counter for a container returns zero it is not mean that the container is empty.
        Thus, item counter may be used for statistical purposes only.
    */
    template <typename Type>
    struct item_counter {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type item_counter ;
        };
        //@endcond
    };

    /// Special alignment constants for \ref cds::opt::alignment option
    enum special_alignment {
        no_special_alignment = 0,   ///< no special alignment
        cache_line_alignment = 1    ///< use cache line size defined in cds/user_setup/cache_line.h
    };

    /// [value-option] Alignment option setter
    /**
        Alignment for some internal data for containers. May be useful to solve false sharing problem.
        \p Value defines desired alignment and it may be power of two or predefined values defined
        in \ref special_alignment enum.
    */
    template <unsigned int Value>
    struct alignment {
        //@cond
        template <typename Base> struct pack: public Base
        {
            enum { alignment = Value }  ;
        };
        //@endcond
    };

    //@cond
    namespace details {
        template <typename Type, unsigned int Alignment>
        struct alignment_setter {
            typedef typename cds::details::aligned_type< Type, Alignment >::type  type    ;
        };

        template <typename Type>
        struct alignment_setter<Type, no_special_alignment> {
            typedef Type type   ;
        };

        template <typename Type>
        struct alignment_setter<Type, cache_line_alignment> {
            typedef typename cds::details::aligned_type< Type, c_nCacheLineSize >::type  type   ;
        };

    } // namespace details
    //@endcond

    /// [type-option] Generic option setter for statisitcs
    /**
        This option sets a type to gather statistics.
        The option is generic - no predefined type(s) is provided.
        The particular \p Type of statistics depends on internal structure of the object.
    */
    template <typename Type>
    struct stat {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type stat ;
        };
        //@endcond
    };

    /// [type-option] Option setter for C++ memory model
    /**
        The <b>cds</b> library supports following memory ordering constraints for atomic operations in container implementation:
        - v::relaxed_ordering - relaxed C++ memory model. This mode supports full set of memory ordering constraints:
            \p memory_order_relaxed, \p memory_order_acquire, \p memory_order_release and so on.
        - v::sequential_consistent - sequentially consistent C++ memory model (default memory ordering for C++). In
            this mode any memory ordering constraint maps to \p memory_order_seq_cst.

        The \p Type template parameter can be v::relaxed_ordering or v::sequential_consistent.

        You may mix different memory ordering options for different containers: one declare as sequentially consistent,
        another declare as relaxed.
        Usually, v::relaxed_ordering is the default memory ordering for <b>cds</b> containers.
    */
    template <typename Type>
    struct memory_model {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type memory_model ;
        };
        //@endcond
    };

    namespace v {
        /// Relaxed memory ordering model
        /**
            In this memory model the memory constraints are defined according to C++ Memory Model specification.

            See opt::memory_model for explanations
        */
        struct relaxed_ordering {
            //@cond

            // For new C++11 (cds-1.1.0)
            static const CDS_ATOMIC::memory_order memory_order_relaxed    = CDS_ATOMIC::memory_order_relaxed;
            static const CDS_ATOMIC::memory_order memory_order_consume    = CDS_ATOMIC::memory_order_consume;
            static const CDS_ATOMIC::memory_order memory_order_acquire    = CDS_ATOMIC::memory_order_acquire;
            static const CDS_ATOMIC::memory_order memory_order_release    = CDS_ATOMIC::memory_order_release;
            static const CDS_ATOMIC::memory_order memory_order_acq_rel    = CDS_ATOMIC::memory_order_acq_rel;
            static const CDS_ATOMIC::memory_order memory_order_seq_cst    = CDS_ATOMIC::memory_order_seq_cst;
            //@endcond
        };

        /// Sequential consistent memory ordering model
        /**
            In this memory model any memory constraint is equivalent to \p memory_order_seq_cst.

            See opt::memory_model for explanations
        */
        struct sequential_consistent {
            //@cond

            // For new C++11 (cds-1.1.0)
            static const CDS_ATOMIC::memory_order memory_order_relaxed    = CDS_ATOMIC::memory_order_seq_cst;
            static const CDS_ATOMIC::memory_order memory_order_consume    = CDS_ATOMIC::memory_order_seq_cst;
            static const CDS_ATOMIC::memory_order memory_order_acquire    = CDS_ATOMIC::memory_order_seq_cst;
            static const CDS_ATOMIC::memory_order memory_order_release    = CDS_ATOMIC::memory_order_seq_cst;
            static const CDS_ATOMIC::memory_order memory_order_acq_rel    = CDS_ATOMIC::memory_order_seq_cst;
            static const CDS_ATOMIC::memory_order memory_order_seq_cst    = CDS_ATOMIC::memory_order_seq_cst;
            //@endcond
        };
    } // namespace v

    /// [type-option] Base type traits option setter
    /**
        This option setter is intended generally for internal use for type rebinding.
    */
    template <typename Type>
    struct type_traits {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type type_traits ;
        };
        //@endcond
    };

}}  // namespace cds::opt

#ifdef CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
#   include <cds/opt/make_options_var.h>
#else
#   include <cds/opt/make_options_std.h>
#endif

#endif  // #ifndef __CDS_OPT_OPTIONS_H
