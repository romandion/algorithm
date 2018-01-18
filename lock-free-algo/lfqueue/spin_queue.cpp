#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spin_queue.h"

spinlock::spinlock()
{
    locked_ = 0 ;
    waiting_ = 0 ;
}

spinlock::~spinlock()
{
    locked_ = 0 ;
    waiting_ = 0 ;
}

void spinlock::lock()
{
    int32_t val = atomic_inc(&locked_) ;
    int idx = 0 ;
    while(val != waiting_)
    {
        ++idx ;
    }
}

void spinlock::unlock()
{
    atomic_inc(&waiting_) ;
}


spin_queue::spin_queue()
{
    dummy_.next = NULL ;
    tail_ = head_ = &dummy_ ;    
}

spin_queue::~spin_queue()
{
    dummy_.next = NULL ;
    tail_ = head_ = &dummy_ ;
}

void spin_queue::enqueue(node * n)
{
    if(n == NULL)
        return ;
    n->next = NULL ;
    tail_lock_.lock() ;

    tail_->next = n ;
    tail_ = n ; 
    tail_lock_.unlock() ;
}

spin_queue::node * spin_queue::dequeue()
{
    node * n = head_->next ;
    if(n == NULL)
        return NULL ;


    return NULL ;
}



