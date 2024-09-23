/* config.h
  Manually edited by Peter Miller 
  version 1.0 - for TDM-Gcc 9.2.0 x64 on windows 10
  version 1.1 - for GCC 14.1.0 x64 (using ucrt runtime) on Windows, only other changes were to use an internal random number generator (USE_INTERNAL_RNG) and some buffer sizes.
			  - on a few simple tests version 1.1 was between 1.2 and 3.2* the speed of version 1.0 
			  - tests were
				- wmawk2 "BEGIN{for(i=0;i<100000000;++i)s+=0.5; print s/100000000}"
					went from 7.08 secs in v1.0 to 3.05 secs in v1.1 (*2.3)
				- wmawk2 "BEGIN{for(i=0;i<100000000;++i)s+=rand(); print s/100000000}"
					went from 9.5 to 3.31 secs (*2.9)
				- wmawk2 "BEGIN{for(i=0;i<1000000;++i)s=s \"z\";print length(s)}"
					went from 41.55 to 13.44 secs (*3.1)
				- wmawk2 "BEGIN{for(i=0;i<1000000;++i)s=s \"z\";print gsub(/z/,\"a\",s)}"
					went from 42.11 to 13.21 secs (*3.2)
			 - it also adds a FILEBUFSIZE #define to set the filebuffer size:
				- wmawk2 "END{print NR}" C:\PMi\csv-files\csvfunbig.csv should give 579486318 (csvfunbig.csv is 12,267,217,518 bytes)
					went from 1:14.94 in 1.0 to 1:01.88 with FILEBUFSIZE not defined,  0:47.70 with FILEBUFSIZE=8192 (which seemed to be the optimum on the system tested which has an SSD).
				- wmawk2 "END{print NR}" <C:\PMi\csv-files\csvfunbig.csv
					went from 1:13.50 with 1v0 to 47.53 with 1v1 (1.5*)
				- wmawk2 "{print $0 >\"out\"}" C:\PMi\csv-files\csvfunbig.csv
				    went from 3:24.50 to 2:31.40 (1.34*)
				- wmawk2 "{print $0}" C:\PMi\csv-files\csvfunbig.csv >out
					went from 3:14.60 to 2:43.11 (*1.2)
			- and it adds #define FILEBUFSIZE_INTERACTIVE  (set to 32768) which sets the maximum interactive (editable during entry - typically stdin) line length.
			  note <control>-Z on a line by itself gives EOF on an interactive input (<control>-Z at some other point in a line does not create an EOF).
				- while all other buffers grow as required this one is fixed (its implemented by the compilers stdio library) 32768 is the largest value for other windows programs I found.

 */
#define MAWK_EXTRA_VERSION_INFO "Windows Github Version 1.1 x64 by Peter Miller" /* define for additional text in --version. Date/time built, compiler used etc will be added when compiled */


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

/* if USE_INTERNAL_RNG is defined then use an internal Random number generator - note this version uses a different generator to that in the original mawk2 code */
#define USE_INTERNAL_RNG

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
#define PACKAGE_VERSION "1.0" 

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1
