/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#ifndef __CDS_OPT_HASH_H
#define __CDS_OPT_HASH_H

#include <cds/opt/options.h>
#include <cds/details/hash_functor_selector.h>

namespace cds { namespace opt {

    /// [type-option] Option setter for a hash function
    /**
        This option setter specifies hash function used in unordered containers.

        The default value  of template argument \p Functor is \p cds::opt::v::hash
        that is synonym for <tt>std::hash</tt> implementation of standard library.
        If standard C++ library of the compiler you are using does not provide TR1 implementation
        the \p cds library automatically selects <tt>boost::hash</tt>.
    */
    template <typename Functor>
    struct hash {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Functor hash ;
        };
        //@endcond
    };

    namespace v {
        //@cond
        using cds::details::hash ;

        /// Metafunction selecting default hash implementation
        /**
            The metafunction selects appropriate hash functor implementation.
            If \p Hash is not equal to opt::none, then result of metafunction is \p Hash.
            Otherwise, the result is <tt> std::hash<Q> </tt> or <tt> boost::hash<Q> </tt>
            depending of compiler you use.

            Note that default hash function like <tt> std::hash<Q> </tt> or <tt> boost::hash<Q> </tt>
            is generally not suitable for complex type \p Q and its derivatives.
            You should manually provide particular hash functor for such types.
        */
        template <typename Hash>
        struct hash_selector
        {
            typedef Hash    type    ;   ///< resulting implementation of hash functor
        };

        template <>
        struct hash_selector<opt::none>
        {
            struct type {
                template <typename Q>
                size_t operator()( const Q& key ) const
                {
                    return hash<Q>( key ) ;
                }
            };
        };
        //@endcond
    }   // namespace v

}} // namespace cds::opt

#endif // #ifndef __CDS_OPT_HASH_H


