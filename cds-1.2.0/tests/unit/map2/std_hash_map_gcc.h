/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#ifndef __CDSUNIT_STD_HASH_MAP_GCC_H
#define __CDSUNIT_STD_HASH_MAP_GCC_H

#include <unordered_map>
#include <cds/ref.h>

namespace map2 {

    template <typename Key, typename Value, typename Lock, class Alloc = CDS_DEFAULT_ALLOCATOR>
    class StdHashMap
        : public std::unordered_map<
            Key, Value
            , std::hash<Key>
            , std::equal_to<Key>
            , Alloc
        >
    {
    public:
        Lock m_lock    ;
        typedef cds::lock::Auto<Lock> AutoLock ;
        typedef std::unordered_map<
            Key, Value
            , std::hash<Key>
            , std::equal_to<Key>
            , Alloc
        >   base_class ;
    public:
        typedef typename base_class::mapped_type value_type ;

        StdHashMap( size_t nMapSize, size_t nLoadFactor )
        {}

        bool find( const Key& key )
        {
            AutoLock al( m_lock )    ;
            return base_class::find( key ) != base_class::end() ;
        }

        bool insert( const Key& key, const Value& val )
        {
            AutoLock al( m_lock )    ;
            return base_class::insert( typename base_class::value_type(key, val)).second ;
        }

        template <typename T, typename Func>
        bool insert( const Key& key, const T& val, Func func )
        {
            AutoLock al( m_lock )    ;
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( typename base_class::value_type(key, Value() )) ;
            if ( pRet.second ) {
                cds::unref(func)( pRet.first->second, val ) ;
                return true ;
            }
            return false;
        }

        template <typename T, typename Func>
        std::pair<bool, bool> ensure( const T& key, Func func )
        {
            AutoLock al( m_lock )   ;
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( typename base_class::value_type( key, Value() )) ;
            if ( pRet.second ) {
                cds::unref(func)( true, *pRet.first ) ;
                return std::make_pair( true, true ) ;
            }
            else {
                cds::unref(func)( false, *pRet.first ) ;
                return std::make_pair( true, false ) ;
            }
        }

        bool erase( const Key& key )
        {
            AutoLock al( m_lock )   ;
            return base_class::erase( key ) != 0 ;
        }

        template <typename T, typename Func>
        bool erase( const T& key, Func func )
        {
            AutoLock al( m_lock )   ;
            typename base_class::iterator it = base_class::find( key ) ;
            if ( it != base_class::end() ) {
                cds::unref(func)( *it )   ;
                return base_class::erase( key ) != 0    ;
            }
            return false ;
        }

        std::ostream& dump( std::ostream& stm ) { return stm; }
    };
}   // namespace map2

#endif  // #ifndef __CDSUNIT_STD_HASH_MAP_GCC_H
