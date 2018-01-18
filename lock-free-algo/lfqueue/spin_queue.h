
#ifndef __SPIN_QUEUE_H
#define __SPIN_QUEUE_H 1

#include "atomic.h"

class spinlock{
public:
    spinlock() ;
    ~spinlock() ;

    void lock() ;
    void unlock() ;
private:
    volatile int32_t locked_ ;
    volatile int32_t waiting_ ;
} ;

class spin_queue{
public:
    typedef struct __st_node{    
        struct __st_node * next ;
    } node;

    spin_queue() ;
    ~spin_queue() ;

    void enqueue(node * n) ;
    node * dequeue() ;

private:
    node dummy_ ;
    node * head_ ;
    node * tail_ ;

    spinlock head_lock_ ;
    spinlock tail_lock_ ;
} ;


#endif  /** __SPIN_QUEUE_H */

