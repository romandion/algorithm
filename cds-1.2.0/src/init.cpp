/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#include <cds/init.h>
#include <cds/cxx11_atomic.h>

#if CDS_OS_INTERFACE == CDS_OSI_WINDOWS
#   if CDS_COMPILER == CDS_COMPILER_MSVC
#       include <cds/threading/details/msvc_manager.h>
#   endif
#   include <cds/threading/details/wintls_manager.h>
#else   // CDS_OS_INTERFACE != CDS_OSI_WINDOWS
#   if CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG
#       include <cds/threading/details/gcc_manager.h>
#   endif
#   include <cds/threading/details/pthread_manager.h>
#endif

namespace cds {

    CDS_EXPORT_API CDS_ATOMIC::atomic<size_t> threading::ThreadData::s_nLastUsedProcNo(0) ;
    CDS_EXPORT_API size_t threading::ThreadData::s_nProcCount = 1  ;

#if CDS_OS_INTERFACE == CDS_OSI_WINDOWS
    CDS_EXPORT_API DWORD cds::threading::wintls::Manager::Holder::m_key = TLS_OUT_OF_INDEXES ;
#else
    pthread_key_t threading::pthread::Manager::Holder::m_key ;
#endif

    namespace details {
        static CDS_ATOMIC::atomic<size_t> s_nInitCallCount(0)   ;

        bool CDS_EXPORT_API init_first_call()
        {
            return s_nInitCallCount.fetch_add(1, CDS_ATOMIC::memory_order_relaxed) == 0 ;
        }

        bool CDS_EXPORT_API fini_last_call()
        {
            if ( s_nInitCallCount.fetch_sub( 1, CDS_ATOMIC::memory_order_relaxed ) == 1 ) {
                CDS_ATOMIC::atomic_thread_fence( CDS_ATOMIC::memory_order_release ) ;
                return true ;
            }
            return false ;
        }
    } // namespace details

}   // namespace cds
