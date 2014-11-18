#ifndef _MYTYPES_H_
#define _MYTYPES_H_


#if defined(__GNUC__)
#include <stdint.h>
#endif

#ifdef __cplusplus
#include <string>
using namespace std;
typedef bool                     tBool;
#else
typedef enum { false=0, true=1 } tBool;
typedef char * string;
#endif

typedef unsigned char  tByte;
typedef char           tChar;

typedef tByte          tUInt8;
typedef tChar          tSInt8;
typedef tSInt8         tInt8;

#if defined(__GNUC__)
typedef uint16_t       tUInt16;
typedef int16_t        tSInt16;
#else
typedef unsigned short int  tUInt16;
typedef short int           tSInt16;
#endif
typedef tSInt16        tInt16;

#if defined(__GNUC__)
typedef uint32_t       tUInt32;
typedef int32_t        tSInt32;
#else
typedef unsigned int   tUInt32;
typedef int            tSInt32;
#endif
typedef tSInt32        tInt32;

#if defined(__GNUC__)
typedef uint64_t       tUInt64;
typedef int64_t        tSInt64;
#else
typedef unsigned _int64 tUInt64;
typedef _int64          tSInt64;
#endif
typedef tSInt64        tInt64;


#ifndef _GLOB_H
/*
  Definition taken from /usr/include/glob.h
*/
  typedef __SIZE_TYPE__ size_t;
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef UNDEFINED
#define UNDEFINED   ((tUInt32) (-1))
#endif

/*
   In order to avoid warning about unused parameters, we can
   use the following macro to force the compiler to ignore.
*/
#include "unused.h"


#endif /* _MYTYPES_H_ */
