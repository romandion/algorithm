
#ifndef __INT_H
#define __INT_H 1

#if defined(WIN32) || defined(_WINDOWS)

typedef char int8_t ;
typedef unsigned char uint8_t ;

typedef short int int16_t ;
typedef unsigned short int uint16_t ;

typedef int int32_t ;
typedef unsigned int uint32_t ;

typedef __int64 int64_t ;
typedef unsigned __int64 uint64_t ;

#else
#include <stdint.h>
#endif


#endif  /** __INT_H */

