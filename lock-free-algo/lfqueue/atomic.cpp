#include "atomic.h"
#include <windows.h>


int32_t atomic_inc(volatile int32_t * addend)
{
    return InterlockedIncrement((volatile LONG *)addend) ;
}

int64_t atomic_inc64(volatile int64_t * addend)
{
    //return InterlockedIncrement64(addend) ;
    return 0 ;
}

int32_t atomic_dec(volatile int32_t * addend)
{
    return InterlockedDecrement((volatile LONG *)addend) ;
}

int64_t atomic_dec(volatile int64_t * addend)
{
    //return InterlockedDecrement64(addend) ;
    return 0 ;
}
/*
int32_t atomic_add(volatile int32_t * addend , int32_t value)
{
    return _InterlockedAdd((volatile LONG *)addend , value) ;
}

int64_t atomic_add64(volatile int64_t * addend , int64_t value)
{
    return _InterlockedAdd64(addend , value) ;
}
*/

int32_t atomic_swap(volatile int32_t * target , int32_t value)
{
    return InterlockedExchange((volatile LONG *)target , value) ;
}

int64_t atomic_swap64(volatile int64_t * target , int64_t value)
{
    //return InterlockedExchange64(target , value) ;
    return 0 ;
}

void * atomic_swap64(volatile void ** target , void * value)
{
    //return InterlockedExchangePointer(target , value) ;
    return NULL ;
}

int32_t atomic_cas(volatile int32_t * target , int32_t exchange , int32_t comparand)
{
    return InterlockedCompareExchange((volatile LONG *)target , exchange , comparand) ;
}

int64_t atomic_cas64(volatile int64_t * target , int64_t exchange , int64_t comparand)
{
    //return InterlockedCompareExchange64(target , exchange , comparand) ;
    return 0 ;
}

void * atomic_casptr(volatile  void ** target , void * exchange , void * comparand)
{
    return InterlockedCompareExchangePointer((volatile  PVOID *)target , exchange , comparand) ;
}
