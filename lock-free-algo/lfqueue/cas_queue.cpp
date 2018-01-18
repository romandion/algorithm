
#include "cas_queue.h"
#include "atomic.h"


cas_queue::cas_queue()
{
    dummy_.next = NULL ;
    head_ = &dummy_ ;
    tail_ = &dummy_ ;
}

cas_queue::~cas_queue()
{
    dummy_.next = NULL ;
    head_ = &dummy_ ;
    tail_ = &dummy_ ;
}

/*
void cas_queue::enqueue(node * n)
{
    if(n == NULL)
        return ;

    n->next = NULL ;
    node * p = (node *)tail_ ;
    node * old = p ;
    do{
        while(p->next != NULL)
            p = p->next ;            
    } while(atomic_casptr((volatile void **)&tail_->next , n , NULL) != NULL) ;
    atomic_casptr((volatile void **)&tail_ , n , old) ;
}

cas_queue::node * cas_queue::dequeue()
{
    node * p = NULL ;
    do{
        p = (node *)head_ ;
        if(p->next == NULL)
            return NULL ;           
    }while(atomic_casptr((volatile void **)&head_ , p->next , p) != p) ;

    if(p == tail_)
    {
        //队列即将为空，唯一的元素要被弹出。
    }

    return p->next ;
}
*/

void cas_queue::enqueue(node * n)
{
    if(n == NULL)
        return ;

    node *next , *last ;

    while(true)
    {
        last = (node *)tail_ ;
        next = last->next ;

        if(last == tail_)
        {
            if(next == NULL)
            {
                if(atomic_casptr((volatile void **)&last->next , n , next) == next)
                    break ;            
            }
            else
            {
                atomic_casptr((volatile void **)&tail_ , next , last) ;
            }
        }
    }//end while true

    atomic_casptr((volatile void **)&tail_ , n , last) ;
}

cas_queue::node * cas_queue::dequeue()
{
    node * first , *last , *next ;


    while(true)
    {
        first = (node *)head_ ;
        last = (node *)tail_ ;

        next = first->next ;

        if(first == head_)
        {
            if(first == last)
            {
                if(next == NULL)
                    return NULL ;

                atomic_casptr((volatile void **)&tail_ , next , last) ;            
            }
            else
            {
                if(atomic_casptr((volatile void **)&head_ , next , first) == first)
                    break ;            
            }
        }
    }//end while true

    return next ;
}


