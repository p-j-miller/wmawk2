

#ifndef MAWK_INT_H
#define MAWK_INT_H
#if defined(__MSVCRT__) && !defined(__USE_MINGW_ANSI_STDIO)
#define __USE_MINGW_ANSI_STDIO 1 /* So mingw uses its printf not msvcrt */
#endif
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h> /* defines PRI64 etc */
#include <math.h>
#include "config.h"

extern const double MAX_INT_DOUBLE ;  /* 2^53-1 */
extern const double MIN_INT_DOUBLE ;  /* -2^53+1 */

/* can be represented EXACTLY as an integer with 8 byte double */
#define is_int_double(x) ((x)==floor(x)&& (x)<=MAX_INT_DOUBLE\
                          &&(x)>=MIN_INT_DOUBLE)

extern const double INT64_MAX_DOUBLE ;
extern const double INT64_MIN_DOUBLE ;
extern const double UINT64_MAX_DOUBLE ;

uint64_t  d_to_u64(double) ;
int64_t  d_to_i64(double) ;
int d_to_int(double) ;

extern const int have_long64 ;

#if LONG_MAX == 0x7fffffffffffffffL
#define LONG64    1
#define LDFMT   "%ld"
#elif LONG_MAX == 0x7fffffffL
#define LONG32    1
#ifdef __STDC_VERSION__ /* if using c99 or above */
#define LDFMT   "%"PRId64 
#else
#define LDFMT   "%lld"
#endif
#else
#error "unexpected value for LONG_MAX"
#endif

#endif /* MAWK_INT_H */
