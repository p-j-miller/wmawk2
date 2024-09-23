
/********************************************
version.c
copyright 1991-95,2014-2016  Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 3, 2007.

If you import elements of this code into another product,
you agree to not name that product mawk.
********************************************/


#include "mawk.h"
#include "config.h"
#include "int.h"

/* mawk 1.9 */
static const char* PATCH_STRING = ".9.6" ;
static const char* DATE_STRING  = "21 Aug 2016" ;

static
const char* const version_string =
  "mawk 1.9%s, %s, Copyright Michael D. Brennan\n\n" ;

/* print VERSION and exit */
void
print_version(void)
{

   printf(version_string, PATCH_STRING, DATE_STRING) ;
#ifdef MAWK_EXTRA_VERSION_INFO
	printf("%s.\nCompiled on %s at %s\n",MAWK_EXTRA_VERSION_INFO,__DATE__,__TIME__); /* announce this version has some modifications by Peter Miller */
#ifdef USE_FAST_STRTOD 
	printf(" Built using fast_strtod()\n");
#endif	
#ifdef MAWK_SYSTIME 
	printf(" Built to include systime() function\n");
#endif	

#ifdef USE_INTERNAL_RNG /* if not defined uses rand() and srand() from stdlib - the internal one is in rand.c which has a period of 2^127 */
 #include "rand.h" /* needed as this changes RAND_MAX */
	printf(" Built to use internal random number generator (not stdlib one) with RAND_MAX=%u\n",RAND_MAX);
#else
	printf(" Built to use the stdlib random number generator which has RAND_MAX=%u\n",RAND_MAX);
#endif
#ifdef FILEBUFSIZE
	printf(" Built to use file buffers of size %u bytes\n",FILEBUFSIZE);
#endif
#if defined(__VERSION__) && defined( __GNUC__)
    printf(" Compiled with gnuc version %s ",__VERSION__);
#ifdef __MINGW64__
	printf(" for Mingw64 ");
#elif define __MINGW32__
	printf(" for Mingw32 ");
#endif
  // _WIN32, _WIN64 and __MSVCRT__ are also defined by gcc */			
#endif    
#if 0
	// check LDFMT is set OK and see how doubles are printed 
	printf("Test of using LDFMT (%s) to print int64_t: 1=" LDFMT ", -1=" LDFMT ", 12345678901234567=" LDFMT "\n", LDFMT,(int64_t)1,(int64_t)-1,(int64_t)12345678901234567);
	printf("Test of double output format: 1.234=%g 1.234e-10=%g 1.234e10=%g\n",1.234,1.234e-10,1.234e10);
#endif
#endif	   
   exit(0) ;
}


