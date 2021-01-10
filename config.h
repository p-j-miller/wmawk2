/* config.h
  Manually edited by Peter Miller for TDM-Gcc 9.2.0 x64 on windows 10
  */
#define MAWK_EXTRA_VERSION_INFO "Windows Github Version 1.0 x64 by Peter Miller" /* define for additional text in --version. Date/time built, compiler used etc will be added when compiled */


#define USE_FAST_STRTOD /* if defined use fast strtod rather than very slow version built into TDM-GCC */

#define MAWK_SYSTIME /* if defined add systime() function that returns seconds since epoch as per mawk 1.3.4 and gawk - but this is an extension not defined in POSIX awk ) */

#ifdef USE_FAST_STRTOD 
double fast_strtod(const char *s,char ** endptr); /* in atof.c */
#endif

/* Define to 1 if you have the declaration of `random', and to 0 if you don't.
   */
#define HAVE_DECL_RANDOM 1

/* Define to 1 if you have the declaration of `srandom', and to 0 if you
   don't. */
#define HAVE_DECL_SRANDOM 1

/* Define to 1 if the system has the type `int64_t'. */
#define HAVE_INT64_T 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 0

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if the system has the type `uint64_t'. */
#define HAVE_UINT64_T 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if the system has the type `__int64_t'. */
#define HAVE___INT64_T 0

/* Define to 1 if the system has the type `__uint64_t'. */
#define HAVE___UINT64_T 0

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "Peter_Miller"

/* Define to the full name of this package. */
#define PACKAGE_NAME "WMAWK2"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "WMawk2 Peter Miller x64"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "WMawk2"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1
