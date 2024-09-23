/* config.h
  Manually edited by Peter Miller 
  version 1.0 - for TDM-Gcc 9.2.0 x64 on windows 10
  version 1.1 - for GCC 14.1.0 x64 (using ucrt runtime) on Windows, only other changes were to use an internal random number generator (USE_INTERNAL_RNG) and some buffer sizes.
						
			- and it adds #define FILEBUFSIZE_INTERACTIVE  (set to 32768) which sets the maximum interactive (editable during entry - typically stdin) line length.
			  note <control>-Z on a line by itself (followed by ENTER) gives EOF on an interactive input (<control>-Z at some other point in a line does not create an EOF).
				- while all other buffers grow as required this one is fixed (its implemented by the compilers stdio library) 32768 is the largest value for other windows programs I found.
				uft8 support means 32768 becomes 8192 utf8 characters maximum.
				
  version 2.0 - added unicode (utf8) support for command line, environment variables, filenames and system/pipe commands (unicode.c)	
		You cannot use utf8 characters as awk variable, array or function names.		
		Changing this more detailed behaviour would break awk programs that expect a string of bytes
		
	- improved memory allocator (better reuse of existing memory, larger allocations from main memory) - the net result is faster and may use less system memory.
	-  added hex constants (0x.... or 0X...) in scripts
	- use of ya_printf() to get faster output that does not rely on compiler printf
	- systime(0) and systime(1) added to get 1us resolution and UTC or localtime. systime() behaves as it always did for backwards compatibility.
	
 */
#define MAWK_EXTRA_VERSION_INFO "Windows Github Version 2.0 x64 by Peter Miller" /* define for additional text in --version. Date/time built, compiler used etc will be added when compiled */


#define USE_FAST_STRTOD /* if defined use fast strtod rather than very slow version built into TDM-GCC */

#define MAWK_SYSTIME /* if defined add systime() function that returns seconds since epoch as per mawk 1.3.4 and gawk - but this is an extension not defined in POSIX awk ) */

#define MAWK_HEX_CONSTANTS /* if defined allow hex constants is 0x9abcd123, note if this is NOT defined this would be the string concatitation of 0 and the variable x9abcd123 (by default "") giving "0" which is probably not what was intended! */

#ifdef USE_FAST_STRTOD 
double fast_strtod(const char *s,char ** endptr); /* in atof.c */
#endif

/* Define to 1 if you have the declaration of `random', and to 0 if you don't.
   */
#define HAVE_DECL_RANDOM 1

/* Define to 1 if you have the declaration of `srandom', and to 0 if you
   don't. */
#define HAVE_DECL_SRANDOM 1

/* if USE_INTERNAL_RNG is defined then use an internal Random number generator - note this version uses a different random number generator to that in the original mawk2 code */
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
