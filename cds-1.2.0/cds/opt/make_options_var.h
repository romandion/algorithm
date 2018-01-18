/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#ifndef __CDS_OPT_MAKE_OPTIONS_VAR_H
#define __CDS_OPT_MAKE_OPTIONS_VAR_H

#ifndef __CDS_OPT_OPTIONS_H
#   error <cds/opt/options.h> must be included instead of <cds/opt/make_options_var.h>
#endif

#define CDS_DECL_OPTIONS1   typename... Options
#define CDS_DECL_OPTIONS2   typename... Options
#define CDS_DECL_OPTIONS3   typename... Options
#define CDS_DECL_OPTIONS4   typename... Options
#define CDS_DECL_OPTIONS5   typename... Options
#define CDS_DECL_OPTIONS6   typename... Options
#define CDS_DECL_OPTIONS7   typename... Options
#define CDS_DECL_OPTIONS8   typename... Options
#define CDS_DECL_OPTIONS9   typename... Options
#define CDS_DECL_OPTIONS10  typename... Options
#define CDS_DECL_OPTIONS11  typename... Options
#define CDS_DECL_OPTIONS12  typename... Options
#define CDS_DECL_OPTIONS13  typename... Options
#define CDS_DECL_OPTIONS14  typename... Options
#define CDS_DECL_OPTIONS15  typename... Options
#define CDS_DECL_OPTIONS16  typename... Options

#define CDS_DECL_OTHER_OPTIONS1   typename... Options2
#define CDS_DECL_OTHER_OPTIONS2   typename... Options2
#define CDS_DECL_OTHER_OPTIONS3   typename... Options2
#define CDS_DECL_OTHER_OPTIONS4   typename... Options2
#define CDS_DECL_OTHER_OPTIONS5   typename... Options2
#define CDS_DECL_OTHER_OPTIONS6   typename... Options2
#define CDS_DECL_OTHER_OPTIONS7   typename... Options2
#define CDS_DECL_OTHER_OPTIONS8   typename... Options2
#define CDS_DECL_OTHER_OPTIONS9   typename... Options2
#define CDS_DECL_OTHER_OPTIONS10  typename... Options2
#define CDS_DECL_OTHER_OPTIONS11  typename... Options2
#define CDS_DECL_OTHER_OPTIONS12  typename... Options2
#define CDS_DECL_OTHER_OPTIONS13  typename... Options2
#define CDS_DECL_OTHER_OPTIONS14  typename... Options2
#define CDS_DECL_OTHER_OPTIONS15  typename... Options2
#define CDS_DECL_OTHER_OPTIONS16  typename... Options2

// for template specializations
#define CDS_SPEC_OPTIONS1   typename... Options
#define CDS_SPEC_OPTIONS2   typename... Options
#define CDS_SPEC_OPTIONS3   typename... Options
#define CDS_SPEC_OPTIONS4   typename... Options
#define CDS_SPEC_OPTIONS5   typename... Options
#define CDS_SPEC_OPTIONS6   typename... Options
#define CDS_SPEC_OPTIONS7   typename... Options
#define CDS_SPEC_OPTIONS8   typename... Options
#define CDS_SPEC_OPTIONS9   typename... Options
#define CDS_SPEC_OPTIONS10  typename... Options
#define CDS_SPEC_OPTIONS11  typename... Options
#define CDS_SPEC_OPTIONS12  typename... Options
#define CDS_SPEC_OPTIONS13  typename... Options
#define CDS_SPEC_OPTIONS14  typename... Options
#define CDS_SPEC_OPTIONS15  typename... Options
#define CDS_SPEC_OPTIONS16  typename... Options

#define CDS_OPTIONS1    Options...
#define CDS_OPTIONS2    Options...
#define CDS_OPTIONS3    Options...
#define CDS_OPTIONS4    Options...
#define CDS_OPTIONS5    Options...
#define CDS_OPTIONS6    Options...
#define CDS_OPTIONS7    Options...
#define CDS_OPTIONS8    Options...
#define CDS_OPTIONS9    Options...
#define CDS_OPTIONS10   Options...
#define CDS_OPTIONS11   Options...
#define CDS_OPTIONS12   Options...
#define CDS_OPTIONS13   Options...
#define CDS_OPTIONS14   Options...
#define CDS_OPTIONS15   Options...
#define CDS_OPTIONS16   Options...
//#define CDS_OPTIONS17   Options...
//#define CDS_OPTIONS18   Options...
//#define CDS_OPTIONS19   Options...
//#define CDS_OPTIONS20   Options...

#define CDS_OTHER_OPTIONS1    Options2...
#define CDS_OTHER_OPTIONS2    Options2...
#define CDS_OTHER_OPTIONS3    Options2...
#define CDS_OTHER_OPTIONS4    Options2...
#define CDS_OTHER_OPTIONS5    Options2...
#define CDS_OTHER_OPTIONS6    Options2...
#define CDS_OTHER_OPTIONS7    Options2...
#define CDS_OTHER_OPTIONS8    Options2...
#define CDS_OTHER_OPTIONS9    Options2...
#define CDS_OTHER_OPTIONS10   Options2...
#define CDS_OTHER_OPTIONS11   Options2...
#define CDS_OTHER_OPTIONS12   Options2...
#define CDS_OTHER_OPTIONS13   Options2...
#define CDS_OTHER_OPTIONS14   Options2...
#define CDS_OTHER_OPTIONS15   Options2...
#define CDS_OTHER_OPTIONS16   Options2...

namespace cds { namespace opt {

    //@cond
    namespace details {
        template <typename OptionList, typename Option>
        struct do_pack
        {
            // Use "pack" member template to pack options
            typedef typename Option::template pack<OptionList> type;
        };

        template <typename ...T> class typelist ;

        template <typename Typelist> struct typelist_head ;
        template <typename Head, typename ...Tail>
        struct typelist_head< typelist<Head, Tail...> > {
            typedef Head type   ;
        };
        template <typename Head>
        struct typelist_head< typelist<Head> > {
            typedef Head type   ;
        };

        template <typename Typelist> struct typelist_tail ;
        template <typename Head, typename ...Tail>
        struct typelist_tail< typelist<Head, Tail...> > {
            typedef typelist<Tail...> type   ;
        };
        template <typename Head>
        struct typelist_tail< typelist<Head> > {
            typedef typelist<> type   ;
        };

        template <typename OptionList, typename Typelist>
        struct make_options_impl {
            typedef typename make_options_impl<
                typename do_pack<
                    OptionList,
                    typename typelist_head< Typelist >::type
                >::type,
                typename typelist_tail<Typelist>::type
            >::type type ;
        };

        template <typename OptionList>
        struct make_options_impl<OptionList, typelist<> > {
            typedef OptionList type ;
        };
    }   // namespace details
    //@endcond

    /// make_options metafunction
    /**
        The metafunction converts option list \p Options to traits structure.
        The result of metafunction is \p type.

        Template parameter \p OptionList is default option set (default traits).
        \p Options is option list.
    */
    template <typename OptionList, typename... Options>
    struct make_options {
#ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined type ;   ///< Result of the metafunction
#else
        typedef typename details::make_options_impl< OptionList, details::typelist<Options...> >::type type  ;
#endif
    };

    //@cond
    namespace details {
        template <typename... Options>
        struct find_type_traits_option  ;

        template <>
        struct find_type_traits_option<> {
            typedef cds::opt::none  type ;
        };

        template <typename Any>
        struct find_type_traits_option< Any > {
            typedef cds::opt::none type ;
        };

        template <typename Any>
        struct find_type_traits_option< cds::opt::type_traits< Any > > {
            typedef Any type ;
        };

        template <typename Any, typename... Options>
        struct find_type_traits_option< cds::opt::type_traits< Any >, Options... > {
            typedef Any type ;
        };

        template <typename Any, typename... Options>
        struct find_type_traits_option< Any, Options... > {
            typedef typename find_type_traits_option< Options... >::type type ;
        };
    } // namespace details
    //@endcond

    /// Metafunction to find opt::type_traits option in \p Options list
    /**
        If \p Options contains opt::type_traits option then it is the metafunction result.
        Otherwise the result is \p DefaultOptons.
    */
    template <typename DefaultOptions, typename... Options>
    struct find_type_traits {
        typedef typename select_default< typename details::find_type_traits_option<Options...>::type, DefaultOptions>::type type ;  ///< Metafunction result
    };

}}  // namespace cds::opt

#endif // #ifndef __CDS_OPT_MAKE_OPTIONS_STD_H
