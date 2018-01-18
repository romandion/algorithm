/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#ifndef __CDS_CONTAINER_MICHAEL_DEQUE_H
#define __CDS_CONTAINER_MICHAEL_DEQUE_H

#include <cds/intrusive/michael_deque.h>
#include <cds/details/trivial_assign.h>

namespace cds { namespace container {

    //@cond
    namespace details {
        template <typename GC, typename T, CDS_DECL_OPTIONS7>
        struct make_michael_deque
        {
            typedef GC gc           ;
            typedef T   value_type  ;

            struct default_options
            {
                typedef cds::backoff::empty                         back_off    ;
                typedef cds::atomicity::empty_item_counter          item_counter;
                typedef cds::intrusive::michael_deque::dummy_stat   stat       ;
                typedef cds::opt::v::relaxed_ordering               memory_model;
                enum { alignment = cds::opt::cache_line_alignment }      ;
                typedef CDS_DEFAULT_ALLOCATOR           allocator   ;
            };

            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< default_options, CDS_OPTIONS7 >::type
                ,CDS_OPTIONS7
            >::type   options ;

            struct node_type: public cds::intrusive::michael_deque::node< gc >
            {
                value_type  m_value ;
                node_type()
                {}
                node_type( const value_type& val )
                    : m_value( val )
                {}
            };

            typedef typename options::allocator::template rebind<node_type>::other allocator_type   ;
            typedef cds::details::Allocator< node_type, allocator_type >           cxx_allocator    ;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode ) ;
                }
            }   ;

            typedef cds::intrusive::MichaelDeque< gc,
                node_type
                ,cds::intrusive::opt::hook<
                    cds::intrusive::michael_deque::base_hook< cds::opt::gc<gc> >
                >
                ,cds::opt::back_off< typename options::back_off >
                ,cds::intrusive::opt::disposer< node_deallocator >
                ,cds::opt::item_counter< typename options::item_counter >
                ,cds::opt::stat< typename options::stat >
                ,cds::opt::alignment< options::alignment >
                ,cds::opt::memory_model< typename options::memory_model >
            > type ;
        };
    }
    //@endcond

    /// Michael's deque
    /** @ingroup cds_nonintrusive_deque

        Implementation of Michael's deque algorithm.

        \par Source:
            [2003] Maged Michael "CAS-based Lock-free Algorithm for Shared Deque"

        <b>Short description</b> (from Michael's paper)

            The deque is represented as a doubly-linked list. Each node in the list contains two link pointers,
            \p pRight and \p pLeft, and a data field. A shared variable, \p Anchor, holds the two anchor
            pointers to the leftmost and rightmost nodes in the list, if any, and a three-value
            status tag. Anchor must fit in a memory block that can be read and manipulated
            using CAS or LL/SC, atomically. Initially both anchor pointers have null values
            and the status tag holds the value stable, indicating an empty deque.

            The status tag serves to indicate if the deque is in an unstable state. When
            a process finds the deque in an unstable state, it must first attempt to take it
            to a stable state before attempting its own operation.

            The algorithm can use 64bit CAS. Instead of a pointer the node contains two
            31bit link indices + one bit for status tag;
            this trick allows use 64bit CAS to manipulate \p Anchor. Internal mapper
            (based on intrusive::MichaelHashSet intrusive container)
            reflects link indices to item pointers. The maximum number of item in
            the deque is limited by <tt>2**31 - 1</tt> that is practically unbounded.

        Template arguments:
        - \p GC - garbage collector type: gc::HP, gc::PTB. Note that gc::HRC is <b>NOT</b> supported for this container.
        - \p T is a type stored in the queue. It should be default-constructible, copy-constructible, assignable type.
        - \p Options - options

        Permissible \p Options:
        - opt::allocator - allocator (like \p std::allocator). Default is \ref CDS_DEFAULT_ALLOCATOR.
            Used for item allocation.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter that means no item counting feature
        - opt::stat - the type to gather internal statistics.
            Possible option value are: \ref intrusive::michael_deque::stat, \ref intrusive::michael_deque::dummy_stat,
            user-provided class that supports intrusive::michael_deque::stat interface.
            Default is \ref intrusive::michael_deque::dummy_stat.
        - opt::alignment - the alignment for internal deque data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).
    */
    template <typename GC, typename T, CDS_DECL_OPTIONS7>
    class MichaelDeque:
#ifdef CDS_DOXYGEN_INVOKED
        intrusive::MichaelDeque< GC, intrusive::michael_deque::node< T >, Options... >
#else
        details::make_michael_deque< GC, T, CDS_OPTIONS7 >::type
#endif
    {
        //@cond
        typedef details::make_michael_deque< GC, T, CDS_OPTIONS7 > options   ;
        typedef typename options::type base_class ;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename GC2, typename T2, CDS_DECL_OTHER_OPTIONS7>
        struct rebind {
            typedef MichaelDeque< GC2, T2, CDS_OTHER_OPTIONS7> other   ;   ///< Rebinding result
        };

    public:
        typedef T value_type ; ///< Value type stored in the deque

        typedef typename base_class::gc                 gc              ; ///< Garbage collector used
        typedef typename base_class::back_off           back_off        ; ///< Back-off strategy used
        typedef typename options::allocator_type        allocator_type  ; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename options::options::item_counter item_counter    ; ///< Item counting policy used
        typedef typename options::options::stat         stat            ; ///< Internal statistics policy used
        typedef typename base_class::memory_model       memory_model    ; ///< Memory ordering. See cds::opt::memory_model option

    protected:
        typedef typename options::node_type  node_type   ;   ///< queue node type (derived from intrusive::single_link::node)

        //@cond
        typedef typename options::cxx_allocator     cxx_allocator   ;
        typedef typename options::node_deallocator  node_deallocator;   // deallocate node
        typedef typename base_class::node_traits    node_traits     ;
        //@endcond

    protected:
        ///@cond
        node_type * alloc_node()
        {
            return cxx_allocator().New()    ;
        }
        node_type * alloc_node( const value_type& val )
        {
            return cxx_allocator().New( val )    ;
        }
        void free_node( node_type * p )
        {
            node_deallocator()( p ) ;
        }
        //@endcond

    public:
        /// Default constructor
        /**
            Initializes the deque object that can contain up to <tt>2**16 - 1</tt> items
        */
        MichaelDeque()
        {}

        /// Constructor
        /**
            Initializes the deque object with estimated item count \p nMaxItemCount.
            \p nLoadFactor is a parameter of internal memory mapper based on intrusive::MichaelHashSet;
            see MichaelHashSet ctor for details
        */
        MichaelDeque( unsigned int nMaxItemCount, unsigned int nLoadFactor = 4 )
            : base_class( nMaxItemCount, nLoadFactor )
            {}

        /// Destructor clears the deque
        ~MichaelDeque()
        {}

    public:
        /// Push back (right) side
        /**
            Push new item \p val to right side of the deque.
        */
        bool push_back( value_type const& val )
        {
            node_type * pNode = alloc_node( val )   ;
            if ( base_class::push_back( *pNode ) )
                return true     ;
            free_node( pNode )  ;
            return false ;
        }

        /// Push back (right) side using copy functor
        /**
            \p Func is a functor called to copy value \p data of type \p Type
            which may be differ from type \p T stored in the deque.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(T& dest, Type const& data)
                {
                    // Code to copy \p data to \p dest
                    dest = data ;
                }
            };
            \endcode
            You may use \p boost:ref construction to pass functor \p f by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Type, typename Func>
        bool push_back( Type const& val, Func f )
        {
            node_type * p = alloc_node()    ;
            unref(f)( p->m_value, val )  ;
            if ( base_class::push_back( *p ))
                return true     ;

            free_node( p )  ;
            return false    ;
        }

        /// Push front (left) side
        /**
            Push new item \p val to left side of the deque.
        */
        bool push_front( value_type const& val )
        {
            node_type * pNode = alloc_node( val )   ;
            if ( base_class::push_front( *pNode ) )
                return true     ;
            free_node( pNode )  ;
            return false ;
        }

        /// Push front side using copy functor
        /**
            \p Func is a functor called to copy value \p data of type \p Type
            which may be differ from type \p T stored in the deque.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(T& dest, Type const& data)
                {
                    // Code to copy \p data to \p dest
                    dest = data ;
                }
            };
            \endcode
            You may use \p boost:ref construction to pass functor \p f by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Type, typename Func>
        bool push_front( Type const& val, Func f )
        {
            node_type * p = alloc_node()    ;
            unref(f)( p->m_value, val )    ;
            if ( base_class::push_front( *p ))
                return true     ;

            free_node( p )  ;
            return false    ;
        }

        /// Pops back side, no return value
        /**
            The function returns \p true if the deque has not been empty (in other words, an item has been popped),
            otherwise the function returns \p false.
        */
        bool pop_back()
        {
            return base_class::pop_back() != null_ptr<node_type *>()   ;
        }

        /// Pops back side a value using copy functor
        /**
            \p Func is a functor called to copy value popped to \p dest of type \p Type
            which may be differ from type \p T stored in the deque.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(Type& dest, T const& data)
                {
                    // Code to copy \p data to \p dest
                    dest = data ;
                }
            };
            \endcode
            You may use \p boost:ref construction to pass functor \p f by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Type, typename Func>
        bool pop_back( Type& dest, Func f )
        {
            typename base_class::pop_result res ;
            if ( base_class::do_pop_back( res )) {
                unref(f)( dest, node_traits::to_value_ptr( res.pPopped )->m_value )  ;
                base_class::dispose_result( res )   ;
                return true ;
            }
            return false ;
        }


        /// Pops back side, store value popped into \p dest
        /**
            If deque is not empty, the function returns \p true, \p dest contains copy of
            value popped. The assignment operator for type \ref value_type is invoked.
            If deque is empty, the function returns \p false, \p dest is unchanged.
        */
        bool pop_back( value_type& dest )
        {
            typedef cds::details::trivial_assign<value_type, value_type> functor ;
            return pop_back( dest, functor() )    ;
        }

        /// Pops front side, no return value
        /**
            The function returns \p true if the deque has not been empty (in other words, an item has been popped),
            otherwise the function returns \p false.
        */
        bool pop_front()
        {
            return base_class::pop_front() != null_ptr<node_type *>()   ;
        }

        /// Pops front side a value using copy functor
        /**
            \p Func is a functor called to copy value popped to \p dest of type \p Type
            which may be differ from type \p T stored in the deque.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(Type& dest, T const& data)
                {
                    // Code to copy \p data to \p dest
                    dest = data ;
                }
            };
            \endcode
            You may use \p boost:ref construction to pass functor \p f by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Type, typename Func>
        bool pop_front( Type& dest, Func f )
        {
            typename base_class::pop_result res ;
            if ( base_class::do_pop_front( res )) {
                unref(f)( dest, node_traits::to_value_ptr( res.pPopped )->m_value )  ;
                base_class::dispose_result( res )   ;
                return true ;
            }
            return false ;
        }


        /// Pops front side, store value popped into \p dest
        /**
            If deque is not empty, the function returns \p true, \p dest contains copy of
            value popped. The assignment operator for type \ref value_type is invoked.
            If deque is empty, the function returns \p false, \p dest is unchanged.
        */
        bool pop_front( value_type& dest )
        {
            typedef cds::details::trivial_assign<value_type, value_type> functor ;
            return pop_front( dest, functor() )    ;
        }

        /// Returns deque's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact does not mean that the deque
            is empty. To check deque emptyness use \ref empty() method.
        */
        size_t size() const
        {
            return base_class::size() ;
        }

        /// Checks if the dequeue is empty
        bool empty() const
        {
            return base_class::empty() ;
        }

        /// Clear the deque
        /**
            The function repeatedly calls \ref pop_back until it returns \p NULL.
        */
        void clear()
        {
            return base_class::clear()  ;
        }

        /// Returns reference to internal statistics
        const stat& statistics() const
        {
            return base_class::statistics() ;
        }
    };

}}  // namespace cds::container


#endif // #ifndef __CDS_CONTAINER_MICHAEL_DEQUE_H
