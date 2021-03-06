/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#ifndef __CDS_DETAILS_TRIVIAL_ASSIGN_H
#define __CDS_DETAILS_TRIVIAL_ASSIGN_H

#include <cds/details/defs.h>

//@cond
namespace cds { namespace details {

    template <typename DEST, typename SRC>
    struct trivial_assign
    {
        DEST& operator()( DEST& dest, const SRC& src )
        {
            return dest = src ;
        }
    };
}}  // namespace cds::details
//@endcond

#endif // #ifndef __CDS_DETAILS_TRIVIAL_ASSIGN_H
