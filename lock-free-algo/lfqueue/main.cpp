

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cas_queue.h"

class data : public cas_queue::node{
public:
    data(){
        value = 0 ;    
    }
    int value ;
} ;

void test_cas_queue1()
{
    cas_queue queue ;

    data val ;

    for(int idx = 0 ; idx < 100 ; ++idx)
    {
        val.value = idx ;
        queue.enqueue(&val) ;

        data * n = (data *)queue.dequeue() ;

        if(n->value != idx) 
        {
            printf("error , idx [%d] \n" , idx) ;
            break ;
        }
    }
}


int main(int argc , char * argv[])
{
    test_cas_queue1() ;

    return 0 ;
}
