
/********************************************
print.c
copyright 1991-1993,2014-2016  Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 3, 2007.

If you import elements of this code into another product,
you agree to not name that product mawk.

1/5/2026 Peter Miller - added "fast path's" for common output formats (supported by ya-dconvert) 
********************************************/
#if defined(__MSVCRT__) && !defined(__USE_MINGW_ANSI_STDIO)
#define __USE_MINGW_ANSI_STDIO 1 /* So mingw uses its printf not msvcrt */
#endif

#include "mawk.h"
#include "bi_vars.h"
#include "bi_funct.h"
#include "memory.h"
#include "field.h"
#include "scan.h"
#include "files.h"
#include "int.h"
#include "printf.h"
#include "..\ya-sprintf\ya-dconvert.h"

static void  print_cell(CELL *, FILE *) ;


/* Once execute() starts the sprintf code is (belatedly) the only
   code allowed to use string_buff  */

static void
print_cell(CELL* p, FILE* fp)
{
   size_t len ;

   switch (p->type)
   {
      case C_NOINIT:
	 break ;
      case C_MBSTRN:
      case C_STRING:
      case C_STRNUM:
	 switch (len = string(p)->len)
	 {
	    case 0:
	       break ;
	    case 1:
	       putc(string(p)->str[0], fp) ;
	       break ;

	    default:
	       fwrite(string(p)->str, 1, len, fp) ;
	 }
	 break ;

      case C_DOUBLE:
	 {
	    double d = p->dval ;
	    if (is_int_double(d)) 
			{int64_t di=d;
#if 1 /* PMi: new code with fast "shortcut" - uses routines from ya-dconvert.h, when combined with fast path for doubles below gives 25% speed improvement on test program createcsvbig_print.awk [ vs just using ya_sprintf() ] */
			 if(di==0) putc('0',fp);// 0 is a special case
			 else if( di>0 && di<100000000) // positive & <100000000 = 100,000,000 so 8 digits max here. for(i=1;i<100000000;++i) print() - takes around 30 secs so this will hopefully capture most usage
			 	{uint64_t a=ya_to_BCD8(di);// a has bcd with 1 digit/byte
			 	 int lz=ya_clz(a)>>3; // 1 byte per digit (does not work when di=a=0)
			 	 a<<=(lz<<3); // remove leading zero's 
			 	 if(is_big_endian()) a|=0x0101010101010101u * '0';// convert to ascii
			 	 else a=bswap64(a|(0x0101010101010101u * '0'));
			 	 fwrite(&a,1,8-lz,fp);// no need for an intermediate buffer - just use a directly
			 	}
			 else if (di<0 && di> -10000000) // negative and > 	-10000000 = -10,000,000 so 7 digits max (to leave room for minus sign)
			 	{uint64_t a=ya_to_BCD8(-di);// a has bcd with 1 digit/byte
			 	 int lz=ya_clz(a)>>3; // 1 byte per digit (does not work when di=a=0)
			 	 lz--; // leave space for leading minus sign
			 	 a<<=(lz<<3); // remove leading zero's 
			 	 if(is_big_endian()) a|=0x2d30303030303030;// convert to ascii or - followed by 7 zero's
			 	 else a=bswap64(a|0x2d30303030303030);
			 	 fwrite(&a,1,8-lz,fp);// no need for an intermediate buffer - just use a directly
			 	}			 
			 else
			 	{ // deal with general case
 #if   LONG64
			 	 fprintf(fp, "%ld", di) ;
 #else
             	 fprintf(fp, "%lld", di) ;
 #endif
 				}
#else // original code			
 #if   LONG64
			 fprintf(fp, "%ld", di) ;
 #else
             fprintf(fp, "%lld", di) ;
 #endif
#endif 
			}
	    else 	
			{
#if 1 /* PMi: new code with "fast path" for common cases, ~ 12% faster on test program createcsvbig_print.awk [vs just using ya_sprint() with fpfmt] */
			 /* we have a double to print in OFMT */
			 char *of=string(OFMT)->str;
			 char *f=of;
			 // see if OFMT is %g or %e in which case we can use a "shortcut" for faster execution
			 // parse fmt looking for %e and %g only allow %e, %.nne, %g, %.nng where nn is a decimal number . * as a precision is not allowed here (by awk print syntax)
			 int prec=6;// precision, default of 6,a different number can be specified in fmt
			 if(*f=='%') // all special cases need to start with %
				{f++;
				 if(*f=='.')
					{// precision found, parse it
					 f++;
					 if(*f>='0' && *f<='9')
						{// precision embedded in format
						 prec=*f++ -'0';
						 while(*f>='0' && *f<='9') prec=prec*10+*f++ -'0';
						}
					}
				 char obuff[32];// should be adequate for any reasonable request
				 if((*f=='e' || *f=='g') && f[1]==0 && prec+10<sizeof(obuff)) // +10 allows for "fixed overhead" of conversion i.e. sign, exponent, etc
					{// %e or g [and nothing else following] - we can do this ourselves without using fsprintf()
					 char *e;
					 if(*f=='e')
						e=ya_dconvert_efmt(obuff,p->dval,prec);
					 else
						e=ya_dconvert_gfmt(obuff,p->dval,prec);
					 fwrite(obuff,1,e-obuff,fp);// actually write out result
					 break;
					} 	 	
				}
			 // use fprintf in case user has set OFMT to something we cannot quickly deal with
			 fprintf(fp, string(OFMT)->str, p->dval) ;
#else /* original code */
	         fprintf(fp, string(OFMT)->str, p->dval) ;
#endif		
			}
	 }
	 break ;

      default:
	 bozo("bad cell passed to print_cell") ;
   }
}

/* on entry to bi_print or bi_printf the stack is:

   sp[0] = an integer k
       if ( k < 0 )  output is to a file with name in sp[-1]
       { so open file and sp -= 2 }

   sp[0] = k >= 0 is the number of print args
   sp[-k]   holds the first argument
*/

CELL *
bi_print(CELL *sp)		 /* stack ptr passed in */
{
   register CELL *p ;
   register int k ;
   FILE *fp ;

   k = sp->type ;
   if (k < 0)
   {
      /* k holds redirection */
      if ((--sp)->type < C_STRING)  cast1_to_s(sp) ;
      fp = (FILE *) file_find(string(sp), k) ;
      free_STRING(string(sp)) ;
      k = (--sp)->type ;
      /* k now has number of arguments */
   }
   else	 fp = stdout ;

   if (k)
   {
      p = sp - k ;		 /* clear k variables off the stack */
      sp = p - 1 ;
      k-- ;

      while (k > 0)
      {
	 print_cell(p,fp) ; print_cell(OFS,fp) ;
	 cell_destroy(p) ;
	 p++ ; k-- ;
      }

      print_cell(p, fp) ;  cell_destroy(p) ;
   }
   else
   {				/* print $0 */
      sp-- ;
      print_cell(&field[0], fp) ;
   }

   print_cell(ORS, fp) ;
   if (ferror(fp)) {
       write_error(fp) ;
       mawk_exit(2) ;
   }
   return sp ;
}

/* first argument is the format as a Form* */
CELL *
bi_printf(CELL* sp)
{
   int k ;
   FILE *fp ;
   const Form* form ;

   k = sp->type ;
   if (k < 0) {
      /* k has redirection */
      if ((--sp)->type < C_STRING)  cast1_to_s(sp) ;
      fp = (FILE *) file_find(string(sp), k) ;
      free_STRING(string(sp)) ;
      k = (--sp)->type ;
      /* k is now number of args including format */
   }
   else	 fp = stdout ;

   sp -= k ;			 /* sp points at the format string */
   k-- ;

   form = (const Form*) sp->ptr ;
   do_xprintf(fp, form, sp+1) ;

   /* cleanup arguments on eval stack */
    {
        CELL *p ;
        for (p = sp + 1; k>0; k--, p++)  cell_destroy(p) ;
    }
    return sp - 1 ;
}

/* The format was not a constant string so must be parsed at run-time */
CELL *
bi_printf1(CELL* sp)
{
    int k ;
    FILE *fp ;
    const Form* form ;

    k = sp->type ;
    if (k < 0) {
        /* k has redirection */
        if ((--sp)->type < C_STRING)  cast1_to_s(sp) ;
        fp = (FILE *) file_find(string(sp), k) ;
        free_STRING(string(sp)) ;
        k = (--sp)->type ;
        /* k is now number of args including format */
    }
    else	 fp = stdout ;

    sp -= k ;			 /* sp points at the format string */
    k-- ;

    if (sp->type < C_STRING)  cast1_to_s(sp) ;
    /* parse form doesn't return on error */
    form = parse_form(string(sp)) ;
    if (form->num_args > k) {
        rt_error("not enough arguments passed to printf(\"%s\")",
                 string(sp)->str) ;
    }

    do_xprintf(fp, form, sp+1) ;
    free_STRING(string(sp)) ;

    /* cleanup arguments on eval stack */
    {
        CELL* p ;
        for (p = sp + 1; k; k--, p++)  cell_destroy(p) ;
    }
    return sp - 1 ;
}

/* format is not constant string */
CELL *
bi_sprintf1(CELL* sp)
{
    int argcnt = sp->type ;
    STRING *sval ;
    const Form* form ;

    sp -= argcnt ;		 /* sp points at the format string */
    argcnt-- ;

    if (sp->type != C_STRING)  cast1_to_s(sp) ;
    form = parse_form(string(sp)) ;
    if (form->num_args > argcnt) {
        rt_error("not enough arguments passed to sprintf(\"%s\")",
                 string(sp)->str) ;
    }
    sval = do_xprintf(0, form, sp + 1) ;
    free_STRING(string(sp)) ;
    sp->ptr = (PTR) sval ;  /* sp->type == C_STRING */

    /* cleanup */
    {
        CELL* p ;
        for (p = sp + 1; argcnt; argcnt--, p++)  cell_destroy(p) ;
    }
    return sp ;
}

/* format is constant string parsed to Form* */
CELL *
bi_sprintf(CELL* sp)
{
    int argcnt = sp->type ;
    STRING *sval ;

    sp -= argcnt ;		 /* sp points at the format string */
    argcnt-- ;

    sval = do_xprintf(0, (const Form*) sp->ptr, sp + 1) ;
    sp->type = C_STRING ;
    sp->ptr = (PTR) sval ;

    /* cleanup */
    {
        CELL* p ;
        for (p = sp + 1; argcnt; argcnt--, p++)  cell_destroy(p) ;
    }
    return sp ;
}
