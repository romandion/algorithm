
#ifndef __ATOMIC_H
#define __ATOMIC_H 1

#include "int.h"

/*
    返回加一后的值
*/
int32_t atomic_inc(volatile int32_t * addend) ;
int64_t atomic_inc64(volatile int64_t * addend) ;

/*
    返回减一后的值
*/
int32_t atomic_dec(volatile int32_t * addend) ;
int64_t atomic_dec(volatile int64_t * addend) ;


/*
    返回增加后的值
*/
/*
int32_t atomic_add(volatile int32_t * addend , int32_t value) ;
int64_t atomic_add64(volatile int64_t * addend , int64_t value) ;
*/

/*
    交换值，返回原始值
*/
int32_t atomic_swap(volatile int32_t * target , int32_t value) ;
int64_t atomic_swap64(volatile int64_t * target , int64_t value) ;
void * atomic_swap64(volatile void ** target , void * value) ;

/*
    CAS , 返回原始值
*/
int32_t atomic_cas(volatile int32_t * target , int32_t exchange , int32_t comparand) ;
int64_t atomic_cas64(volatile int64_t * target , int64_t exchange , int64_t comparand) ;
void * atomic_casptr(volatile void * * target , void * exchange , void * comparand) ;

#endif  /** __ATOMIC_H */
