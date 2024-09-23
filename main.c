
/********************************************
main.c
copyright 1991,2014-2016 Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 3, 2007.

If you import elements of this code into another product,
you agree to not name that product mawk.
********************************************/



/*  main.c  */

#include "mawk.h"
#include "init.h"
#include "code.h"
#include "files.h"


int mawk_state ;		 /* 0 is compiling */
int exit_code ;

#if defined(__MINGW64__) || defined(__MINGW32__)
int _dowildcard =-1;  /* for MINGW runtime - force command line expansion as this is not done by the windows cmd.exe shell. */
#endif

int
main(int argc, char **argv)
{
/* set buffering for stdin if needed */
#ifdef FILEBUFSIZE_INTERACTIVE  /* PJM - new constant in sizes.h - sets bigger buffer for interactive file input, this will also be used if stdin is redirected */
   if( _isatty( _fileno( stdin ) ) )
       setvbuf(stdin, NULL,_IOLBF, FILEBUFSIZE_INTERACTIVE);/* set buffer for stdin to size requested, NULL means setvbuf will allocate space for buffer itself */
 #ifdef FILEBUFSIZE  /* PJM - new constant in sizes.h - sets bigger buffer for file i/o (in this case redirected stdin which may improve performance with i/o bound scripts */
   else setvbuf(stdin, NULL, _IOFBF, FILEBUFSIZE);/* set buffer to size requested, NULL means setvbuf will allocate space for buffer itself */
 #endif
#endif 
 
 /* set buffering for stdout if required */
 #ifdef FILEBUFSIZE  /* PJM - new constant in sizes.h - sets bigger buffer for file i/o (in this case redirected stdout which may improve performance with i/o bound scripts */
      if( !_isatty( _fileno( stdout ) ) ) setvbuf(stdout, NULL, _IOFBF, FILEBUFSIZE);/* set buffer to size requested, NULL means setvbuf will allocate space for buffer itself */
 #endif
 
   initialize(argc, argv) ;

   parse() ;

   mawk_state = EXECUTION ;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds="
   execute(execution_start, eval_stack - 1, 0) ; /* eval_stack-1 is correct as always increment sp before push, but GCC 14.1.0 gives a warning here which #programs cause to be ignored */
#pragma GCC diagnostic pop  	
   /* never returns */
   return 0 ;
}

void
mawk_exit(int x)
{
    if (mawk_state == EXECUTION) {
        if (close_all_output() < 0) {
            x = 2 ;
        }
    }
    exit(x) ;
}
