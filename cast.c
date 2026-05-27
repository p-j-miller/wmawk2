
/********************************************
cast.c
copyright 1991,2014-2016 Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 3, 2007.

If you import elements of this code into another product,
you agree to not name that product mawk.

Peter Miller 2/5/2026 added "fast path" to cast1_to_s().

********************************************/




/*  cast.c  */

#include "mawk.h"
#include "field.h"
#include "memory.h"
#include "scan.h"
#include "repl.h"
#include "int.h"
#include "config.h"
#include "..\ya-sprintf\ya-dconvert.h"


int mpow2[NUM_CELL_TYPES] =
{1, 2, 4, 8, 16, 32, 64, 128, 256, 512} ;


/* modern strtod accepts "inf" "nan" and hex numbers
   awk should not  (gawk and mawk agree on this)

   maybe if --posix, but stubbed out for now]
*/

int posix_flag = 0 ;

static
double awk_strtod(const STRING* sval)
{
    double ret = 0.0 ;
    const char* s = sval->str ;
    char* stop ;
    /* eat space ourselves because it makes it easy to eliminate "inf"/"nan" */
    while (scan_code[*(unsigned char*)s] == SC_SPACE)  s++ ;

    switch (scan_code[*(unsigned char*)s])
    {
        case SC_DIGIT:
        case SC_PLUS:
        case SC_MINUS:
        case SC_DOT:
	    errno = 0 ;
#ifdef USE_FAST_STRTOD 
		ret = fast_strtod(s, &stop) ;
#else	    
	    ret = strtod(s, &stop) ;
#endif	    
#if  FPE_TRAPS_ON
	    if (errno && ret != 0.0) {
	        rt_error("overflow converting \"%s\" to double",s) ;
	    }
#endif

	    /* check for hex number */
	    while(s < stop) {
	        if (*s == 'x' || *s == 'X') return 0.0 ;
		s++ ;
	    }

    }

    return ret ;
}

static
double posix_strtod(const STRING* sval)
{
    double ret ;

    errno = 0 ;
#ifdef USE_FAST_STRTOD 
 	ret = fast_strtod(sval->str, 0) ;
#else    
    ret = strtod(sval->str, 0) ;
#endif    
#if  FPE_TRAPS_ON
    if (errno && ret != 0) {
        rt_error("overflow converting \"%s\" to double",sval->str) ;
    }
#endif
    return ret ;
}

void
cast1_to_d(CELL* cp)
{
   switch (cp->type)
   {
      case C_NOINIT:
	 cp->dval = 0.0 ;
	 break ;

      case C_DOUBLE:
	 return ;

      case C_MBSTRN:
      case C_STRING:
	 {
	    STRING* sval = (STRING *) cp->ptr ;

	    cp->dval = posix_flag ? posix_strtod(sval)
	                          : awk_strtod(sval) ;

            free_STRING(sval) ;
	 }
	 break ;

      case C_STRNUM:
	 /* don't need to convert, but do need to free the STRING part */
	 free_STRING(string(cp)) ;
	 break ;


      default:
	 bozo("cast on bad type") ;
   }
   cp->type = C_DOUBLE ;
}


static
STRING*  slow_convfmt(const char* conv, double d, size_t need)
{
    /* don't expect to get here for a reasonable program */
    STRING* ret ;
    char* buffer = (char *)emalloc(need+1) ;
    buffer[need] = 0 ;
    sprintf(buffer, conv, d) ;
    ret =  new_STRING(buffer) ;
    free(buffer) ;
    return ret ;
}

void
cast1_to_s(CELL* cp)
{
   switch (cp->type) {
      case C_NOINIT:
	 cp->ptr = STRING_dup(the_empty_str) ;
	 break ;

      case C_DOUBLE:
      {
        char buffer[1024] ;
	    double d = cp->dval ;
	    if (is_int_double(d)) 
			{int64_t di=(int64_t) d;
#if 1 /* PMi: new code with fast "shortcut" - uses routines from ya-dconvert.h, deals quickly with integers -10,000,000 <int<100,000,000, the rest are handled by ya_sprintf - both fast paths combined give 45 % reduction in test time for createcsvbig_print-cast.awk giving 4.4* overall vs 2v0 */
			 if(di==0) 
			 	{// 0 is a special case
			 	 buffer[0]='0';
			 	 buffer[1]=0;
			 	}
			 else if( di>0 && di<100000000) // positive & <100000000 = 100,000,000 so 8 digits max here. for(i=1;i<100000000;++i) print() - takes around 30 secs so this will hopefully capture most usage
			 	{uint64_t a=ya_to_BCD8(di);// a has bcd with 1 digit/byte
			 	 int lz=ya_clz(a)>>3; // 1 byte per digit (does not work when di=a=0)
			 	 a<<=(lz<<3); // remove leading zero's 
			 	 if(is_big_endian()) a|=0x0101010101010101u * '0';// convert to ascii
			 	 else a=bswap64(a|(0x0101010101010101u * '0'));
			 	 memcpy(buffer,&a,8); // its faster to always copy 8 bytes
			 	 buffer[8-lz]=0; // terminate c string
			 	}
			 else if (di<0 && di> -10000000) // negative and > 	-10000000 = -10,000,000 so 7 digits max (to leave room for minus sign)
			 	{uint64_t a=ya_to_BCD8(-di);// a has bcd with 1 digit/byte
			 	 int lz=ya_clz(a)>>3; // 1 byte per digit (does not work when di=a=0)
			 	 lz--; // leave space for leading minus sign
			 	 a<<=(lz<<3); // remove leading zero's 
			 	 if(is_big_endian()) a|=0x2d30303030303030;// convert to ascii or - followed by 7 zero's
			 	 else a=bswap64(a|0x2d30303030303030);
			 	 memcpy(buffer,&a,8); // its faster to always copy 8 bytes
			 	 buffer[8-lz]=0; // terminate c string
			 	}			 
			 else
			 	{ // deal with general case
 #if   LONG64
			 	 sprintf(buffer,"%ld", di) ;
 #else
             	 sprintf(buffer,"%lld", di) ;
 #endif
 				}
#else // original code				
 #if  LONG64
              sprintf(buffer,"%ld", di) ;
 #else
              sprintf(buffer,"%lld", di) ;
 #endif
#endif
              cp->ptr = new_STRING(buffer) ;
          	}
	    else  
		 {
	      const char* conv = string(CONVFMT)->str ;
	      unsigned used=0 ;
#if 1 /* PMi: new code with "fast path" for common cases - default CONVFMT is %.6g which is one of the cases we do speed up (the other is %e) */
		 /* we have a double to print in CONVFMT */
		 const char *f=conv;
		 // see if CONVFMT is %g or %e in which case we can use a "shortcut" for faster execution
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
			 if((*f=='e' || *f=='g') && f[1]==0 && prec+10<sizeof(buffer)) // +10 allows for "fixed overhead" of conversion i.e. sign, exponent, etc
				{// %e or g [and nothing else following] - we can do this ourselves without using fsprintf()
				 char *e;
				 if(*f=='e')
					e=ya_dconvert_efmt(buffer,d,prec);
				 else
					e=ya_dconvert_gfmt(buffer,d,prec);
				 cp->ptr = new_STRING2(buffer,e-buffer) ;// actually write out result
				 used=1;// flag we have successfully done the conversion
				} 	 	
			}
		 if(used==0)
		 	{// fall back to original code, this still uses ya_sprintf() so benefits from use of the fpfmt algorithm, but ya_sprintf() is much more complex and so slower than the special cases above
		      used = snprintf(buffer, sizeof(buffer), conv, d) ;
		      if ((int) used < 0) 
			  	{
		          rt_error("snprintf bozo (%d)", errno) ;
		      	}
		      if (used > sizeof(buffer)) 
			  	{
		          cp->ptr = slow_convfmt(conv, d, used) ;
		      	}
		      else 
			  	{
		          cp->ptr = new_STRING2(buffer,used) ;
		      	}		 	
		 	}
#else /* original code */ 	      
	      used = snprintf(buffer, 1024, conv, d) ;
	      if ((int) used < 0) 
		  	{
	          rt_error("snprintf bozo (%d)", errno) ;
	      	}
	      if (used > 1024) 
		  	{
	          cp->ptr = slow_convfmt(conv, d, used) ;
	      	}
	      else 
		  	{
	          cp->ptr = new_STRING2(buffer,used) ;
	      	}
#endif	      	
         }
	   break ;
      }

      case C_STRING:
	 return ;

      case C_MBSTRN:
      case C_STRNUM:
	 break ;

      default:
	 bozo("bad type on cast") ;
   }
   cp->type = C_STRING ;
}


void
cast_to_RE(CELL *cp)
{
   register PTR p ;

   if (cp->type < C_STRING)  cast1_to_s(cp) ;

   p = re_compile(string(cp)) ;
   free_STRING(string(cp)) ;
   cp->type = C_RE ;
   cp->ptr = p ;

}

void
cast_for_split(CELL* cp)
{
   static char meta[] = "^$.*+?|[]()" ;
   static char xbuff[] = "\\X" ;
   int c ;
   size_t len ;

   if (cp->type < C_STRING)  cast1_to_s(cp) ;

   if ((len = string(cp)->len) == 1)
   {
      if ((c = string(cp)->str[0]) == ' ')
      {
	 free_STRING(string(cp)) ;
	 cp->type = C_SPACE ;
	 return ;
      }
      else if (c != 0 && strchr(meta, c))
      {
	 xbuff[1] = c ;
	 free_STRING(string(cp)) ;
	 cp->ptr = (PTR) new_STRING(xbuff) ;
      }
   }
   else if (len == 0)
   {
      free_STRING(string(cp)) ;
      cp->type = C_SNULL ;
      return ;
   }

   cast_to_RE(cp) ;
}

/* input: cp-> a CELL of type C_MBSTRN (maybe strnum)
   test it -- casting it to the appropriate type
   which is C_STRING or C_STRNUM

   eliminate some values strtod likes  0x 0X inf nan

*/

void
check_strnum(CELL* cp)
{
    unsigned char *test ;
    char** tp = (char**)&test ;
    unsigned char *s ;
    unsigned char *q ;

    cp->type = C_STRING ;	 /* assume not C_STRNUM */
    s = (unsigned char *) string(cp)->str ;
    q = s + string(cp)->len ;
    while (scan_code[*s] == SC_SPACE)  s++ ;

    switch (scan_code[*s])
    {
        case SC_DIGIT:
        case SC_PLUS:
        case SC_MINUS:
        case SC_DOT:
	    errno = 0 ;
#ifdef USE_FAST_STRTOD 
		cp->dval = fast_strtod((char *) s, tp) ;
#else	    
	    cp->dval = strtod((char *) s, tp) ;
#endif	    
	    /* make overflow and underflow pure string */
	    if (errno || test == s) {
	        errno = 0 ;
		return ;
	    }

	    /* we have a number, but must be all of it .
	       we allow space at back */

	    while(q > test && scan_code[q[-1]] == SC_SPACE) q-- ;

	    if (q != test) return ;
	    /* and finally eliminate hex strings */
	    while(s < q) {
	        if (*s == 'x' || *s == 'X') return ;
		s++ ;
	    }
	    cp->type = C_STRNUM ;
	    return ;

	default:
	      /* not strnum */
	      return ;
   }
}

/* cast a CELL to a replacement cell */

void
cast_to_REPL(CELL* cp)
{
   STRING *sval ;

   if (cp->type < C_STRING)  {
       cast1_to_s(cp) ;
   }
   sval = (STRING *) cp->ptr ;
   /* cp no longer ownes sval */
   cp->type = C_NOINIT ;
   replacement_scan(sval,cp) ;
   free_STRING(sval) ;
}


