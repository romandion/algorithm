
#ifndef __CAS_QUEUE_H
#define __CAS_QUEUE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class cas_queue{
public:
    typedef struct __st_node {
        struct __st_node * next ;    
    } node;

    cas_queue() ;
    ~cas_queue() ;

    void enqueue(node * n) ;
    node * dequeue() ;

    inline bool empty() const {return (dummy_.next == NULL);}
private:
    node dummy_ ;
    volatile node * head_ ;
    volatile node * tail_ ;
} ;


#endif  /** __CAS_QUEUE_H */

