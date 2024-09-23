/********************************************
sizes.h
copyright 1991-1995,2014-2016.  Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 3, 2007.

If you import elements of this code into another product,
you agree to not name that product mawk.
********************************************/


/*  sizes.h  */

#ifndef  SIZES_H
#define  SIZES_H

#define  MAX__INT   0x7fffffff /* 32 bits signed */


#define EVAL_STACK_SIZE  256	/* initial size , can grow */

/*
 * FBANK_SZ, the number of fields at startup, must be a power of 2.
 *
 */
#define  FBANK_SZ	1024
#define  FB_SHIFT	  10	/* lg(FBANK_SZ) */

/*
 * initial size of sprintf buffer
 */
#define  SPRINTF_LIMIT	8192

#define  FILEBUFSIZE  	8192 /* PJM - new constant in sizes.h : if defined set the default buffer size for buffered file i/o - should be a power of 2 > BUFSIZ (512) */
#define FILEBUFSIZE_INTERACTIVE  32768 /* PJM - new constant in sizes.h - if defined sets bigger buffer for interactive file input (if not defined its 4096 for gcc/mingw) */
									   /* note: this is the longest line length that can be typed (or pasted) in as interactive input allows editing the line before typing <ENTER> */
#define  BUFFSZ         8192 /* was 4096 increased to 8192 for consistancy (might give slight speed gain when reading longer awk scripts from a file)*/
#define  FINBUFFSZ      8192
  /* starting buffer size for input files, grows if
     necessary */

#define  HASH_PRIME  113

#define  MAX_COMPILE_ERRORS  5	/* quit if more than 4 errors */

#endif /* SIZES_H */
