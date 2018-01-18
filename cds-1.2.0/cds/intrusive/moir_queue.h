/*
    This file is a part of libcds - Concurrent Data Structures library
    (C) Copyright Maxim Khiszinsky [khizmax at gmail dot com] 2006-2012
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.2.0
*/


#ifndef __CDS_INTRUSIVE_MOIR_QUEUE_H
#define __CDS_INTRUSIVE_MOIR_QUEUE_H

#include <cds/intrusive/msqueue.h>

namespace cds { namespace intrusive {

    template <typename GC, typename T, CDS_DECL_OPTIONS9>
    class MoirQueue: public MSQueue< GC, T, CDS_OPTIONS9 >
    {
        //@cond
        typedef MSQueue< GC, T, CDS_OPTIONS9 > base_class ;
        typedef typename base_class::node_type node_type ;
        //@endcond

    public:
        //@cond
        typedef typename base_class::value_type value_type  ;
        typedef typename base_class::back_off   back_off    ;
        typedef typename base_class::gc         gc          ;
        typedef typename base_class::node_traits node_traits;
        typedef typename base_class::memory_model   memory_model    ;
        //@endcond

        /// Rebind template arguments
        template <typename GC2, typename T2, CDS_DECL_OTHER_OPTIONS9>
        struct rebind {
            typedef MoirQueue< GC2, T2, CDS_OTHER_OPTIONS9> other   ;   ///< Rebinding result
        };

    protected:
        //@cond
        typedef typename base_class::dequeue_result dequeue_result   ;
        typedef typename base_class::node_to_value node_to_value    ;

        bool do_dequeue( dequeue_result& res )
        {
            back_off bkoff  ;

            node_type * pNext    ;
            node_type * h        ;
            while ( true ) {
                h = res.guards.protect( 0, base_class::m_pHead, node_to_value() )  ;
                pNext = res.guards.protect( 1, h->m_pNext, node_to_value() )   ;

                if ( pNext == null_ptr<node_type *>() )
                    return false    ;    // queue is empty

                if ( base_class::m_pHead.compare_exchange_strong( h, pNext, memory_model::memory_order_release, CDS_ATOMIC::memory_order_relaxed )) {
                    node_type * t = base_class::m_pTail.load(memory_model::memory_order_acquire)    ;
                    if ( h == t )
                        base_class::m_pTail.compare_exchange_strong( t, pNext, memory_model::memory_order_release, CDS_ATOMIC::memory_order_relaxed ) ;
                    break ;
                }

                base_class::m_Stat.onDequeueRace()    ;
                bkoff()    ;
            }

            --base_class::m_ItemCounter     ;
            base_class::m_Stat.onDequeue()  ;

            res.pHead = h   ;
            res.pNext = pNext   ;
            return true     ;
        }
        //@endcond

    public:
        /// Dequeues a value from the queue
        /**
            See warning about item disposing in \ref MSQueue::dequeue.
        */
        value_type * dequeue()
        {
            dequeue_result res  ;
            if ( do_dequeue( res )) {
                base_class::dispose_result( res )   ;
                return node_traits::to_value_ptr( *res.pNext )  ;
            }
            return null_ptr<value_type *>()     ;
        }

        /// Synonym for \ref dequeue function
        value_type * pop()
        {
            return dequeue()    ;
        }
    };

}} // namespace cds::intrusive

#endif // #ifndef __CDS_INTRUSIVE_MOIR_QUEUE_H
