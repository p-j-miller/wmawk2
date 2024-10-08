
/********************************************
bi_funct.h
copyright 1991,2014-2016 Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 3, 2007.

If you import elements of this code into another product,
you agree to not name that product mawk.
********************************************/



#ifndef  BI_FUNCT_H
#define  BI_FUNCT_H  1

#include "symtype.h"

extern BI_REC  bi_funct_trad[] ;
extern BI_REC  bi_funct_new[] ;

void  bi_init(void) ;

CELL * bi_print(CELL *) ;
CELL * bi_printf(CELL *) ;
CELL * bi_printf1(CELL *) ;
CELL * bi_length(CELL *) ;
CELL* bi_alength(CELL*) ;   /* length/size of an array */
CELL * bi_index(CELL *) ;
CELL * bi_substr(CELL *) ;
CELL * bi_sprintf(CELL *) ;
CELL * bi_sprintf1(CELL *) ;
CELL * bi_split(CELL *) ;
CELL * bi_match(CELL *) ;
CELL * bi_getline(CELL *) ;
CELL * bi_sub(CELL *) ;
CELL * bi_gsub(CELL *) ;
CELL * bi_toupper(CELL*) ;
CELL * bi_tolower(CELL*) ;

/* builtin arith functions */
CELL * bi_sin(CELL *) ;
CELL * bi_cos(CELL *) ;
CELL * bi_atan2(CELL *) ;
CELL * bi_log(CELL *) ;
CELL * bi_exp(CELL *) ;
CELL * bi_int(CELL *) ;
CELL * bi_sqrt(CELL *) ;
CELL * bi_srand(CELL *) ;
CELL * bi_rand(CELL *) ;
#ifdef MAWK_SYSTIME
CELL * bi_systime(CELL *sp);
#endif

/* other builtins */
CELL * bi_close(CELL *) ;
CELL * bi_system(CELL *) ;
CELL * bi_fflush(CELL *) ;

#endif  /* BI_FUNCT_H  */

