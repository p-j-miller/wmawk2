 
 /* fast_atof by Peter Miller 23/1/2020
    This is original code (C) 2020 Peter Miller.
    
    This atof also returns a "good number" flag so its NOT a drop in replacement for the standard atof.
    
	 The number of fp operations has been minimised so the accuracy should be good  [ see below for results with long double vs just using doubles] 
	 The code should be robust to anything thrown at it (lots of leading of trailing zeros, extremely large numbers of digits, etc ).
	 As well as floating point numbers this also accepts NAN and INF (case does not matter).
	 
	 As the name suggests its also written to be fast (much faster than the built in strtod or atof library functions at least for mingw64 ).
	 
				  
 */   


/*----------------------------------------------------------------------------
 * Copyright (c) 2020 Peter Miller
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h> /* for bool */
#include <stdint.h>  /* for int64_t etc */
#include <math.h>    /* for NAN, INFINITY */

// #define DEBUG
#ifdef DEBUG
#include <stdio.h>
#endif

/* ieee floating point maths limits:
   double (64 bits) max 				1.797 e + 308
   					min 				2.225 e-308
   					min denormalised	4.94 e-324
   					sig digits			15-17
   	float (32 bit)	max					3.4e38
   					min					1.17e-38
   					min denormalised	1.4e-45
   					sig digits			6-9

Also note that 0x0fff ffff ffff ffff =  1,152,921,504,606,846,975  	, so 18 digits easily fit into a 64 bit unsigned (with 4 bits spare) - which is enough for a double mantissa.
2^64= 18,446,744,073,709,551,616 
0xffff ffff = 	4,294,967,295 so 9 digits fits with a 32 bit unsigned (with 2 bits spare) which is NOT enough for a float mantissa			

*/   					
double fast_atof(const char *s,bool * not_number);
double fast_atof_nan(const char *s);// like fast_atof, but returns NAN if whole string is not a valid number
double fast_strtod(const char *s,char ** endptr);
float fast_strtof(const char *s,char **endptr); // if endptr != NULL returns 1st character thats not in the number
   					
static const int maxExponent = 308;	/* Largest possible base 10 for a double exponent. (must match array below) */
static const int maxfExponent = 38;	/* Largest possible base 10 for a float exponent. (must match array below) */

static long double const powersOf10[] =
                {
                    1e0L,   1e1L,   1e2L,   1e3L,   1e4L,   1e5L,   1e6L,   1e7L,   1e8L,    1e9L,
                    1e10L,  1e11L,  1e12L,  1e13L,  1e14L,  1e15L,  1e16L,  1e17L,  1e18L,  1e19L,
                    1e20L,  1e21L,  1e22L,  1e23L,  1e24L,  1e25L,  1e26L,  1e27L,  1e28L,  1e29L,
                    1e30L,  1e31L,  1e32L,  1e33L,  1e34L,  1e35L,  1e36L,  1e37L,  1e38L,  1e39L,
                    1e40L,  1e41L,  1e42L,  1e43L,  1e44L,  1e45L,  1e46L,  1e47L,  1e48L,  1e49L,
                    1e50L,  1e51L,  1e52L,  1e53L,  1e54L,  1e55L,  1e56L,  1e57L,  1e58L,  1e59L,
                    1e60L,  1e61L,  1e62L,  1e63L,  1e64L,  1e65L,  1e66L,  1e67L,  1e68L,  1e69L,
                    1e70L,  1e71L,  1e72L,  1e73L,  1e74L,  1e75L,  1e76L,  1e77L,  1e78L,  1e79L,
                    1e80L,  1e81L,  1e82L,  1e83L,  1e84L,  1e85L,  1e86L,  1e87L,  1e88L,  1e89L,
                    1e90L,  1e91L,  1e92L,  1e93L,  1e94L,  1e95L,  1e96L,  1e97L,  1e98L,  1e99L,
                    1e100L, 1e101L, 1e102L, 1e103L, 1e104L, 1e105L, 1e106L, 1e107L, 1e108L, 1e109L,
                    1e110L, 1e111L, 1e112L, 1e113L, 1e114L, 1e115L, 1e116L, 1e117L, 1e118L, 1e119L,
                    1e120L, 1e121L, 1e122L, 1e123L, 1e124L, 1e125L, 1e126L, 1e127L, 1e128L, 1e129L,
                    1e130L, 1e131L, 1e132L, 1e133L, 1e134L, 1e135L, 1e136L, 1e137L, 1e138L, 1e139L,
                    1e140L, 1e141L, 1e142L, 1e143L, 1e144L, 1e145L, 1e146L, 1e147L, 1e148L, 1e149L,
                    1e150L, 1e151L, 1e152L, 1e153L, 1e154L, 1e155L, 1e156L, 1e157L, 1e158L, 1e159L,
                    1e160L, 1e161L, 1e162L, 1e163L, 1e164L, 1e165L, 1e166L, 1e167L, 1e168L, 1e169L,
                    1e170L, 1e171L, 1e172L, 1e173L, 1e174L, 1e175L, 1e176L, 1e177L, 1e178L, 1e179L,
                    1e180L, 1e181L, 1e182L, 1e183L, 1e184L, 1e185L, 1e186L, 1e187L, 1e188L, 1e189L,
                    1e190L, 1e191L, 1e192L, 1e193L, 1e194L, 1e195L, 1e196L, 1e197L, 1e198L, 1e199L,
                    1e200L, 1e201L, 1e202L, 1e203L, 1e204L, 1e205L, 1e206L, 1e207L, 1e208L, 1e209L,
                    1e210L, 1e211L, 1e212L, 1e213L, 1e214L, 1e215L, 1e216L, 1e217L, 1e218L, 1e219L,
                    1e220L, 1e221L, 1e222L, 1e223L, 1e224L, 1e225L, 1e226L, 1e227L, 1e228L, 1e229L,
                    1e230L, 1e231L, 1e232L, 1e233L, 1e234L, 1e235L, 1e236L, 1e237L, 1e238L, 1e239L,
                    1e240L, 1e241L, 1e242L, 1e243L, 1e244L, 1e245L, 1e246L, 1e247L, 1e248L, 1e249L,
                    1e250L, 1e251L, 1e252L, 1e253L, 1e254L, 1e255L, 1e256L, 1e257L, 1e258L, 1e259L,
                    1e260L, 1e261L, 1e262L, 1e263L, 1e264L, 1e265L, 1e266L, 1e267L, 1e268L, 1e269L,
                    1e270L, 1e271L, 1e272L, 1e273L, 1e274L, 1e275L, 1e276L, 1e277L, 1e278L, 1e279L,
                    1e280L, 1e281L, 1e282L, 1e283L, 1e284L, 1e285L, 1e286L, 1e287L, 1e288L, 1e289L,
                    1e290L, 1e291L, 1e292L, 1e293L, 1e294L, 1e295L, 1e296L, 1e297L, 1e298L, 1e299L,
                    1e300L, 1e301L, 1e302L, 1e303L, 1e304L, 1e305L, 1e306L, 1e307L, 1e308L
                };

static double const dblpowersOf10[] = /* always double */

                {
                    1e0,   1e1,   1e2,   1e3,   1e4,   1e5,   1e6,   1e7,   1e8,    1e9,
                    1e10,  1e11,  1e12,  1e13,  1e14,  1e15,  1e16,  1e17,  1e18,  1e19,
                    1e20,  1e21,  1e22,  1e23,  1e24,  1e25,  1e26,  1e27,  1e28,  1e29,
                    1e30,  1e31,  1e32,  1e33,  1e34,  1e35,  1e36,  1e37,  1e38,  1e39,
                    1e40,  1e41,  1e42,  1e43,  1e44,  1e45,  1e46,  1e47,  1e48,  1e49,
                    1e50,  1e51,  1e52,  1e53,  1e54,  1e55,  1e56,  1e57,  1e58,  1e59,
                    1e60,  1e61,  1e62,  1e63,  1e64,  1e65,  1e66,  1e67,  1e68,  1e69,
                    1e70,  1e71,  1e72,  1e73,  1e74,  1e75,  1e76,  1e77,  1e78,  1e79,
                    1e80,  1e81,  1e82,  1e83,  1e84,  1e85,  1e86,  1e87,  1e88,  1e89,
                    1e90,  1e91,  1e92,  1e93,  1e94,  1e95,  1e96,  1e97,  1e98,  1e99,
                    1e100, 1e101, 1e102, 1e103, 1e104, 1e105, 1e106, 1e107, 1e108, 1e109,
                    1e110, 1e111, 1e112, 1e113, 1e114, 1e115, 1e116, 1e117, 1e118, 1e119,
                    1e120, 1e121, 1e122, 1e123, 1e124, 1e125, 1e126, 1e127, 1e128, 1e129,
                    1e130, 1e131, 1e132, 1e133, 1e134, 1e135, 1e136, 1e137, 1e138, 1e139,
                    1e140, 1e141, 1e142, 1e143, 1e144, 1e145, 1e146, 1e147, 1e148, 1e149,
                    1e150, 1e151, 1e152, 1e153, 1e154, 1e155, 1e156, 1e157, 1e158, 1e159,
                    1e160, 1e161, 1e162, 1e163, 1e164, 1e165, 1e166, 1e167, 1e168, 1e169,
                    1e170, 1e171, 1e172, 1e173, 1e174, 1e175, 1e176, 1e177, 1e178, 1e179,
                    1e180, 1e181, 1e182, 1e183, 1e184, 1e185, 1e186, 1e187, 1e188, 1e189,
                    1e190, 1e191, 1e192, 1e193, 1e194, 1e195, 1e196, 1e197, 1e198, 1e199,
                    1e200, 1e201, 1e202, 1e203, 1e204, 1e205, 1e206, 1e207, 1e208, 1e209,
                    1e210, 1e211, 1e212, 1e213, 1e214, 1e215, 1e216, 1e217, 1e218, 1e219,
                    1e220, 1e221, 1e222, 1e223, 1e224, 1e225, 1e226, 1e227, 1e228, 1e229,
                    1e230, 1e231, 1e232, 1e233, 1e234, 1e235, 1e236, 1e237, 1e238, 1e239,
                    1e240, 1e241, 1e242, 1e243, 1e244, 1e245, 1e246, 1e247, 1e248, 1e249,
                    1e250, 1e251, 1e252, 1e253, 1e254, 1e255, 1e256, 1e257, 1e258, 1e259,
                    1e260, 1e261, 1e262, 1e263, 1e264, 1e265, 1e266, 1e267, 1e268, 1e269,
                    1e270, 1e271, 1e272, 1e273, 1e274, 1e275, 1e276, 1e277, 1e278, 1e279,
                    1e280, 1e281, 1e282, 1e283, 1e284, 1e285, 1e286, 1e287, 1e288, 1e289,
                    1e290, 1e291, 1e292, 1e293, 1e294, 1e295, 1e296, 1e297, 1e298, 1e299,
                    1e300, 1e301, 1e302, 1e303, 1e304, 1e305, 1e306, 1e307, 1e308
                };

static float const fltpowersOf10[] = /* always float */

                {
                    1e0f,   1e1f,   1e2f,   1e3f,   1e4f,   1e5f,   1e6f,   1e7f,   1e8f,    1e9f,
                    1e10f,  1e11f,  1e12f,  1e13f,  1e14f,  1e15f,  1e16f,  1e17f,  1e18f,  1e19f,
                    1e20f,  1e21f,  1e22f,  1e23f,  1e24f,  1e25f,  1e26f,  1e27f,  1e28f,  1e29f,
                    1e30f,  1e31f,  1e32f,  1e33f,  1e34f,  1e35f,  1e36f,  1e37f,  1e38f
                };
                
static uint64_t u64powersOf10[]=
				{
					UINT64_C(1), 	// 10^ 0
					UINT64_C(10), 	// 10^1
					UINT64_C(100), 	// 10^2
					UINT64_C(1000),	// 10^3
					UINT64_C(10000),// 10^4
					UINT64_C(100000),// 5
					UINT64_C(1000000),// 6
					UINT64_C(10000000),// 7
					UINT64_C(100000000),// 8
					UINT64_C(1000000000),// 9   
					UINT64_C(10000000000),// 10 
					UINT64_C(100000000000),// 11  
					UINT64_C(1000000000000),// 12   
					UINT64_C(10000000000000),// 13
					UINT64_C(100000000000000),// 14
					UINT64_C(1000000000000000), // 15
					UINT64_C(10000000000000000), // 16 
					UINT64_C(100000000000000000), // 17 
					UINT64_C(1000000000000000000)  // 18 [ largest possible 10^19 gives compiler error (overflow) ]
				};

static uint32_t u32powersOf10[]=
				{
					UINT32_C(1), 	// 10^ 0
					UINT32_C(10), 	// 10^1
					UINT32_C(100), 	// 10^2
					UINT32_C(1000),	// 10^3
					UINT32_C(10000),// 10^4
					UINT32_C(100000),// 5
					UINT32_C(1000000),// 6
					UINT32_C(10000000),// 7
					UINT32_C(100000000),// 8
					UINT32_C(1000000000),// 9   [ largest possible 10^10 gives compiler error (overflow) ]
				};
/*
 *----------------------------------------------------------------------
 *
 * double fast_strtod(const char *s,char **endptr)
 *
 *	This procedure converts a floating-point number from an ASCII
 *	decimal representation to internal double-precision format.
 *  Also accepts "NaN", "Inf" and "Infinity" (any mix of case) which return NAN and INFINITY
 * Results:
 *	The return value is the floating-point equivalent of string.
 *	*endptr is set to the first character after the valid number 
 *
 * If endptr == NULL it is ignored.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
 static const int maxdigits=19; // see above this is the largest possible in a uint64 

 
 double fast_strtod(const char *s,char **endptr) // if endptr != NULL returns 1st character thats not in the number
 {
  long double dr;
  bool sign=false,expsign=false,got_number=false;
  uint64_t r=0; // mantissa
  int exp=0,rexp=0;
  int nos_mant_digits=0;
  const char *se=s; // string end - candidate for endptr
#ifdef DEBUG
  fprintf(stderr,"strtod(%s):\n",s);
#endif    
  while(isspace(*s)) ++s; // skip initial whitespace	
  // deal with leading sign
  if(*s=='+') ++s;
  else if(*s=='-')
  	{sign=true;
  	 ++s;
    }
  // NAN is a special case - NAN is  signed in the input but always returns NAN
  if((*s=='n' || *s=='N') && (s[1]=='a' || s[1]=='A') && (s[2]=='n' || s[2]=='N'))
  	{if(endptr!=NULL) *endptr=(char *)s+3;// 3 for NAN
  	 return NAN;
  	}    
  // INF or Infinity is a special case - and is signed
  if((*s=='i' || *s=='I') && (s[1]=='n' || s[1]=='N') && (s[2]=='f' || s[2]=='F'))
  	{s+=3;// INF
  	 if((*s=='i' || *s=='I') && (s[1]=='n' || s[1]=='N') && (s[2]=='i' || s[2]=='I') && (s[3]=='t' || s[3]=='T') && (s[4]=='y' || s[4]=='Y') )
  	  	s+=5; // "Infinity" is 5 more chars (inity) than "inf"
  	 if(endptr!=NULL) *endptr=(char *)s;
  	 if(sign) return -INFINITY;
  	 return INFINITY;
  	}      
  // skip leading zeros
  while(*s=='0')
  	{got_number=true; // have a number (0)
	 ++s;
	}
  // now read rest of the mantissa	
  while(isdigit(*s))
  	{ got_number=true; // have a valid number
	  if(nos_mant_digits<maxdigits)
	  	{ r=r*10+(*s-'0');
		  nos_mant_digits++;	
		}
	  else
	  	{ 
		  if(nos_mant_digits==maxdigits)
	  	  		{if(*s >='5') r++; // use 1st "ignored digit" to round to nearest
	  	  		 nos_mant_digits++;
	  	  		}
		  if(exp<2*maxExponent)
		      exp++; // cannot actually capture digits as more than 18 but keep track of decimal point, trap at 2*maxExponent ensures we don't overflow exp when given a number with a silly number of digits (that would overflow a double)
		}
	++s;
	}
  // now look for optional decimal point (and fractional bit of mantissa)
  if(*s=='.')
  	{ // got decimal point, skip and then look for fractional bit
  	 ++s;
  	 if(r==0)
  	 	{// number is zero at present, so deal with leading zeros in fractional bit of mantissa
  	 	 while(*s=='0')
  	 	 	{got_number=true;
  	 	 	 ++s;
  	 	 	 if(exp > -2*maxExponent)
  	 	 	 	exp--; // test avoids issues with silly number of leading zeros
  	 	    }
  	 	}
  	 // now process the rest of the fractional bit of the mantissa
	 while(isdigit(*s))
	 	{got_number=true;
#if 1	 	
  	 	// see if the whole remaining fractional bit is "0", if so can just skip. This speeds up some conversions (and slows others) but more importantly it ensures 1, 1.0, 1.00 & 1.15, 1.150, 1.1500 etc give exactly the same result
		 
	 	 if(*s=='0')
	 		{// got a zero, see if all remaining numbers in mantissa are zero
	 		 const char *s0=s;
	 	  	 while(*s0=='0') ++s0;
	 	  	 if(!isdigit(*s0))
	 			{// was all zero's, just skip them
		 	 	 s=s0;
		 	 	 break;
				}
			}		 	
#endif			
	 	 if(nos_mant_digits<maxdigits)
	  			{ r=r*10+(*s-'0');
		  		  nos_mant_digits++;	
		  		  exp--;
				}
		 else
	  			{  // cannot actually capture digits as more than 18, so just ignore them (expect for next digit which we use for rounding)
	  			  if(nos_mant_digits==maxdigits ) 
	  			  		{if(*s >='5') r++; // use 1st "ignored digit" to round to nearest
	  			  		 nos_mant_digits++;
	  			  		}
				}
		++s;
		}
 	}
  // got all of mantissa - see if its a valid number, if not we are done
  if(!got_number)
 	{if(endptr!=NULL) *endptr=(char *)se;
#ifdef DEBUG
 	fprintf(stderr," strtod returns 0 (invalid number)\n"); 
#endif  	
 	 return 0;
 	}	
  se=s; // update to reflect end of a valid mantissa
  // now see if we have an  exponent
  if(*s=='e' || *s=='E')
  	{// have exponent, optional sign is 1st
  	 ++s ; // skip 'e'
  	 if(*s=='+') ++s;
  	 else if(*s=='-') 
  	 	{expsign=true;
  	 	 ++s;
  	 	}
  	 while(isdigit(*s))
	   	{if(rexp<=2*maxExponent)
		   rexp=rexp*10+(*s - '0');  // if statement clips at a value that will result in +/-inf but will not overflow int
		 ++s;  
		 se=s; // update to reflect end of a valid exponent (e[+-]digit+)
		}
	}
 if(endptr!=NULL) *endptr=(char *)se; // we now know the end of the number - so save it now (means we can have multiple returns going forward without having to worry about this)	
 if(expsign) rexp=-rexp;	
 rexp+=exp; // add in correct to exponent from mantissa processing
#if 1 /* if 0 removes the optimisations which just results in slower code - there is no loss of accuracy with these optimisations */
 if(rexp>0 && rexp+nos_mant_digits<=maxdigits)
 	{// optimisation: can do all calculations using uint64 which is fastish and exact
 	 r*=u64powersOf10[rexp];
 	 rexp=0;// will just convert to double and return
 	}
 else if(rexp<0 && rexp >= -15 && nos_mant_digits<=15 )
 	{// in this region we can use double rather than long double as 10^15 is exact as a double (another speed optimisation, but one than thats common and therefore worthwhile)
 	 if(sign) return -(r/dblpowersOf10[-rexp]); // negative exponent means we divide by powers of 10
 	 else return r/dblpowersOf10[-rexp];
	}
#endif
 // calculate dr=(long double)r*powl(10,rexp), but by using a lookup table of powers of 10 for speed and accuracy, and using long doubles to ensure accuracy.
 if(rexp>0)
 	{if(rexp>maxExponent)
		{// we have defininaly overflowed
		 if(sign) return -INFINITY;
 		 return INFINITY;
 		}
 	 dr=r;
 	 dr*=powersOf10[rexp]; // as mantissa >= 1 this may overflow, but thats OK.
	}
 else if(rexp<0)
 	{// need to take care here as mantissa is > 1 so even dividing by 10^maxExponent may not be enough, here we all division by upto 10^2*maxExponet is is by far enough
	 rexp= -rexp;
	 exp=rexp;
	 if(rexp>maxExponent)
	 	{
 		 rexp=maxExponent;
 		 exp-=maxExponent; // any excess which we will also need to divide by (if its > 0)
 		}
 	  else exp=0;	
 	  dr=r;
 	  dr/=powersOf10[rexp]; // negative exponent means we divide by powers of 10
 	  if(exp>0)
 	  	{dr/=powersOf10[exp]; // divide by some more, we should only be dividing by max 10^18 as we only have 18 sig figs in mantissa (plus a few more if we consider creation of denormalised numbers)
 	    }
	}	
 else
 	{// special case, rexp==0
#if 1
	 dr=r;
#else 	
	 // do not need to use long double here, so we use double for speed.
 	 if(sign) return -((double)r); // r is unsigned so cannot do -r !
 	 else return (double) r;
#endif 	 
	}

 if(sign) dr= -dr;
#ifdef DEBUG
 // while this is the normal return there are several earlier return possibilities, which this will not print for (sorry).
 fprintf(stderr," strtod returns %.18g (rexp=%d, exp=%d)\n",(double)dr,rexp,exp); 
#endif 
#if 0
 double d0=dr;
 long double diff=dr-(long double)d0;
 double d1=nexttoward(d0,dr);
 long double diff1=dr-(long double)d1;
 if (fabsl(diff1)<fabsl(diff)) return d1; // <= gives more round trip errors with my fast_strtod()
 else return d0;
#elif 0
 double d0=dr; // try 1 bit each way
 long double diff=dr-(long double)d0;
 double d1=nextafter(d0,0.0);// down 1 bit
 long double diff1=dr-(long double)d1;
 double d2=nextafter(d0,d0*1.1);// up 1 bit
 long double diff2=dr-(long double)d2;
 if (fabsl(diff1)>fabsl(diff2)) 
 	{diff1=diff2; // d2 is better than d1
 	 d1=d2;
 	}
 if (fabsl(diff1)<fabsl(diff)) return d1; 		
 return d0;
#else
 return (double)dr; // this should be only rounding [it will be round to even] - to this point we have used integer maths (exact) or long doubles (very nearly exact).
#endif 
}
 
 
/*
 *----------------------------------------------------------------------
 *
 * double fast_atof(const char *s,bool * not_number) :
 *
 *	This procedure converts a floating-point number from an ASCII
 *	decimal representation to internal double-precision format.
 *  Also accepts "NaN", "Inf" and "Infinity" (any mix of case) which return NAN and INFINITY
 * Results:
 *	The return value is the floating-point equivalent of string.
 *	If a terminating character is found before any floating-point
 *	digits, then zero is returned and *not_number is set to true (otherwise its set to false);
 *
 * If not_number == NULL it is ignored.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */ 
double fast_atof(const char *s,bool * not_number)
{char *se;
 double r;
 r=fast_strtod(s,&se);
 if(not_number != NULL) *not_number = (se==(char *)s); // if strtod found no number not_number is set to true
 return r;
}

double fast_atof_nan(const char *s)// like fast_atof, but returns NAN if the string does not start with a valid number (fast_atof returns 0)
{char *se;
 double r;
 r=fast_strtod(s,&se);
 if(se==s) return NAN; // if string does not contain a number return NAN
 return r;
}


/*
 *----------------------------------------------------------------------
 *
 * float fast_strtof(const char *s,char **endptr)
 *
 *	This procedure converts a floating-point number from an ASCII
 *	decimal representation to internal single-precision format.
 *  Also accepts "NaN", "Inf" and "Infinity" (any mix of case) which return NAN and INFINITY
 * Results:
 *	The return value is the floating-point equivalent of string.
 *	*endptr is set to the first character after the valid number 
 *
 * If endptr == NULL it is ignored.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */


#if defined(__SIZEOF_POINTER__ ) && __SIZEOF_POINTER__ == 8 /* compiling for x86_64 , this is defined for gcc, if its not defined the build version uisng u32's which is the best default  */ 
 /* could also have used __WIN64__, WIN64, __WIN64, __X86_64__ which all appear to only be defined for 64 bit compiles */
 /* version of fast_strtof() using uint64 - this passes the test suite with no errors */
 /* tests with nsort show this is faster than the version below which tries to use uint32's as much as possible when compiled for x64  
    When compiled for 64 bits, this code takes 1.203 secs to sort demo1M.csv, whereas the code below based on u32's takes 1.406 secs. 
    When compiled for 32 bits, this version using u64 takes 2.093 secs, while the version using u32 takes 1.532 secs
    above #if statement should automatically compile the fastest version.
 */   
const int fast_strtof_u=64; // tell rest of system we are using u64's (only useful for diagnostics/debugging)

static const int maxfdigits=18; // see above this is the largest possible in a uint64
float fast_strtof(const char *s,char **endptr) // if endptr != NULL returns 1st character thats not in the number
 {
  double dr; // may need to use double precision to get an accurate float - we try hard below to avoid this either by uisng just a uint32, or just by using float's
  bool sign=false,expsign=false,got_number=false;
  uint64_t r=0; // mantissa, uint32 can hold 9 digits which is NOT enough for a float
  int exp=0,rexp=0;
  int nos_mant_digits=0;
  const char *se=s; // string end - candidate for endptr
#ifdef DEBUG
  fprintf(stderr,"strtof(%s):\n",s);
#endif    
  while(isspace(*s)) ++s; // skip initial whitespace	
  // deal with leading sign
  if(*s=='+') ++s;
  else if(*s=='-')
  	{sign=true;
  	 ++s;
    }
  // NAN is a special case - NAN is  signed in the input but always returns NAN
  if((*s=='n' || *s=='N') && (s[1]=='a' || s[1]=='A') && (s[2]=='n' || s[2]=='N'))
  	{if(endptr!=NULL) *endptr=(char *)s+3;// 3 for NAN
  	 return NAN;
  	}    
  // INF or Infinity is a special case - and is signed
  if((*s=='i' || *s=='I') && (s[1]=='n' || s[1]=='N') && (s[2]=='f' || s[2]=='F'))
  	{s+=3;// INF
  	 if((*s=='i' || *s=='I') && (s[1]=='n' || s[1]=='N') && (s[2]=='i' || s[2]=='I') && (s[3]=='t' || s[3]=='T') && (s[4]=='y' || s[4]=='Y') )
  	  	s+=5; // "Infinity" is 5 more chars (inity) than "inf"
  	 if(endptr!=NULL) *endptr=(char *)s;
  	 if(sign) return -INFINITY;
  	 return INFINITY;
  	}      
  // skip leading zeros
  while(*s=='0')
  	{got_number=true; // have a number (0)
	 ++s;
	}
  // now read rest of the mantissa	
  while(isdigit(*s))
  	{ got_number=true; // have a valid number
	  if(nos_mant_digits<maxfdigits)
	  	{ r=r*10+(*s-'0');
		  nos_mant_digits++;	
		}
	  else
	  	{ if(exp<2*maxfExponent)
		      exp++; // cannot actually capture digits as more than 9 but keep track of decimal point, trap at 2*maxExponent ensures we don't overflow exp when given a number with a silly number of digits (that would overflow a double)
		}
	++s;
	}
  // now look for optional decimal point (and fractional bit of mantissa)
  if(*s=='.')
  	{ // got decimal point, skip and then look for fractional bit
  	 ++s;
  	 if(r==0)
  	 	{// number is zero at present, so deal with leading zeros in fractional bit of mantissa
  	 	 while(*s=='0')
  	 	 	{got_number=true;
  	 	 	 ++s;
  	 	 	 if(exp > -2*maxfExponent)
  	 	 	 	exp--; // test avoids issues with silly number of leading zeros
  	 	    }
  	 	}
  	 // now process the rest of the fractional bit of the mantissa
	 while(isdigit(*s))
	 	{got_number=true;
	 	 if(nos_mant_digits<maxfdigits)
	  			{ r=r*10+(*s-'0');
		  		  nos_mant_digits++;	
		  		  exp--;
				}
		 else
	  			{  // cannot actually capture digits as more than 9, so just ignore them
				}
		++s;
		}
 	}
  // got all of mantissa - see if its a valid number, if not we are done
  if(!got_number)
 	{if(endptr!=NULL) *endptr=(char *)se;
#ifdef DEBUG
 	fprintf(stderr," strtof returns 0 (invalid number)\n"); 
#endif  	
 	 return 0;
 	}	
  se=s; // update to reflect end of a valid mantissa
  // now see if we have an  exponent
  if(*s=='e' || *s=='E')
  	{// have exponent, optional sign is 1st
  	 ++s ; // skip 'e'
  	 if(*s=='+') ++s;
  	 else if(*s=='-') 
  	 	{expsign=true;
  	 	 ++s;
  	 	}
  	 while(isdigit(*s))
	   	{if(rexp<=2*maxfExponent)
		   rexp=rexp*10+(*s - '0');  // if statement clips at a value that will result in +/-inf but will not overflow int
		 ++s;  
		 se=s; // update to reflect end of a valid exponent (e[+-]digit+)
		}
	}
 if(endptr!=NULL) *endptr=(char *)se; // we now know the end of the number - so save it now (means we can have multiple returns going forward without having to worry about this)	
 if(expsign) rexp=-rexp;	
 rexp+=exp; // add in correct to exponent from mantissa processing
#if 1 /* if 0 removes the optimisations which just results in slower code - there is no loss of accuracy with these optimisations */
 if(rexp>0 && rexp+nos_mant_digits<=9)
 	{// optimisation: can do all calculations using uint32 which is exact and fast
 	 uint32_t r32=r;	 
 	 r32*=u32powersOf10[rexp];
 	 if(sign) return -((float)r32); // negative exponent means we divide by powers of 10
 	 else return (float)r32;
 	}
 else if(rexp<0 && rexp >= -6 && nos_mant_digits<=6 )
 	{// in this region we can use float rather than double as 10^6 is exact as a float (another speed optimisation, but one than thats common and therefore worthwhile)
 	 if(sign) return -((float)r/fltpowersOf10[-rexp]); // negative exponent means we divide by powers of 10
 	 else return (float)r/fltpowersOf10[-rexp];
	}
#endif
 // calculate dr=(float)r*pow(10,rexp), but by using a lookup table of powers of 10 for speed and accuracy, and using doubles to ensure accuracy.
 if(rexp>0)
 	{if(rexp>maxfExponent)
		{// we have defininaly overflowed
		 if(sign) return -INFINITY;
 		 return INFINITY;
 		}
 	 dr=(double)r*dblpowersOf10[rexp]; // as mantissa >= 1 this may overflow, but thats OK.
	}
 else if(rexp<0)
 	{// need to take care here as mantissa is > 1 so even dividing by 10^maxExponent may not be enough, here we all division by upto 10^2*maxExponet is is by far enough
	 rexp= -rexp;
	 exp=rexp;
	 if(rexp>maxfExponent)
	 	{
 		 rexp=maxfExponent;
 		 exp-=maxfExponent; // any excess which we will also need to divide by (if its > 0)
 		}
 	  else exp=0;	
 	  dr=(double)r/dblpowersOf10[rexp]; // negative exponent means we divide by powers of 10
 	  if(exp>0)
 	  	{dr/=dblpowersOf10[exp]; // divide by some more, we should only be dividing by max 10^18 as we only have 18 sig figs in mantissa (plus a few more if we consider creation of denormalised numbers)
 	    }
	}	
 else
 	{// special case, rexp==0
	 // do not need to use double here, so we use float for speed.
 	 if(sign) return -((float)r); // r is unsigned so cannot do -r !
 	 else return (float) r;
	}

 if(sign) dr= -dr;
#ifdef DEBUG
 // while this is the normal return there are several earlier return possibilities, which this will not print for (sorry).
 fprintf(stderr," strtof returns %.18g (rexp=%d, exp=%d)\n",(double)dr,rexp,exp); 
#endif 
 return (float)dr;
}

#else 

 /* version of fast_strtof() using unit32 */
 /* ======================================*/
 /* I could not get this to work to full accuracy by just using a uint32 mantissa, so the code below uses a uint32 for as long as possible then swaps to a uint64 */
 /* this a number of uint64 multiplies and additions and so is faster that just using a uint64 all the time */
 const int fast_strtof_u=32; // tell rest of system we are using u32's (only useful for diagnostics/debugging)
 static const int maxfdigits=18; // see above this is the largest possible in a uint32
 
float fast_strtof(const char *s,char **endptr) // if endptr != NULL returns 1st character thats not in the number
 {
  double dr; // may need to use double precision to get an accurate float - we try hard below to avoid this either by uisng just a uint32, or just by using float's
  bool sign=false,expsign=false,got_number=false;
  uint_fast32_t r32=0; // mantissa, uint32 can hold 9 digits 
  uint_fast64_t r64=0; // if we get too many digits in mantissa then we swap to using this
  bool usingr64=false; // set to true when we use r64
  int_fast16_t exp=0,rexp=0;
  int_fast16_t nos_mant_digits=0;
  const char *se=s; // string end - candidate for endptr
#ifdef DEBUG
  fprintf(stderr,"strtof(%s):\n",s);
#endif    
  while(isspace(*s)) ++s; // skip initial whitespace	
  // deal with leading sign
  if(*s=='+') ++s;
  else if(*s=='-')
  	{sign=true;
  	 ++s;
    }
  // NAN is a special case - NAN is  signed in the input but always returns NAN
  if((*s=='n' || *s=='N') && (s[1]=='a' || s[1]=='A') && (s[2]=='n' || s[2]=='N'))
  	{if(endptr!=NULL) *endptr=(char *)s+3;// 3 for NAN
  	 return NAN;
  	}    
  // INF or Infinity is a special case - and is signed
  if((*s=='i' || *s=='I') && (s[1]=='n' || s[1]=='N') && (s[2]=='f' || s[2]=='F'))
  	{s+=3;// INF
  	 if((*s=='i' || *s=='I') && (s[1]=='n' || s[1]=='N') && (s[2]=='i' || s[2]=='I') && (s[3]=='t' || s[3]=='T') && (s[4]=='y' || s[4]=='Y') )
  	  	s+=5; // "Infinity" is 5 more chars (inity) than "inf"
  	 if(endptr!=NULL) *endptr=(char *)s;
  	 if(sign) return -INFINITY;
  	 return INFINITY;
  	}      
  // skip leading zeros
  while(*s=='0')
  	{got_number=true; // have a number (0)
	 ++s;
	}
  // now read rest of the mantissa	
  while(isdigit(*s))
  	{ got_number=true; // have a valid number
	  if(!usingr64 && r32<=429496728  )
	  	{ r32=r32*10+(*s-'0');// uint32 can hold upto 4,294,967,295 so we can directly store this extra digit
		  nos_mant_digits++;	
		}
	  else if(nos_mant_digits < maxfdigits)	
	    {if(!usingr64) 
			{r64=r32;// too many digits for a uint32, swap to a uint64
			 usingr64=true;
			}
		 r64=r64*10+(*s-'0'); 
		 nos_mant_digits++;	
		}		
	  else if(exp<2*maxfExponent)
		      exp++; // cannot actually capture digits as more than 9 but keep track of decimal point, trap at 2*maxExponent ensures we don't overflow exp when given a number with a silly number of digits (that would overflow a double)		
	  ++s;
	}
  // now look for optional decimal point (and fractional bit of mantissa)
  if(*s=='.')
  	{ // got decimal point, skip and then look for fractional bit
  	 ++s;
  	 if(r32==0)
  	 	{// number is zero at present, so deal with leading zeros in fractional bit of mantissa
  	 	 while(*s=='0')
  	 	 	{got_number=true;
  	 	 	 ++s;
  	 	 	 if(exp > -2*maxfExponent)
  	 	 	 	exp--; // test avoids issues with silly number of leading zeros
  	 	    }
  	 	}
  	 // now process the rest of the fractional bit of the mantissa
	 while(isdigit(*s))
	 	{got_number=true;
	 	 if(!usingr64 && r32<=429496728 )
	  			{ r32=r32*10+(*s-'0'); // uint32 can hold upto 4,294,967,295 so we can directly store this extra digit
		  		  nos_mant_digits++;	
		  		  exp--;
				}
	  	 else if(nos_mant_digits < maxfdigits)	
	    	{if(!usingr64) 
				{r64=r32;// too many digits for a uint32, swap to a uint64
				 usingr64=true;
				}
		 	 r64=r64*10+(*s-'0'); 
		 	 nos_mant_digits++;	
		 	 exp--;
			}		  			
		 else
	  			{ 
				   // cannot actually capture digits as more than 18, so just ignore them as they after after decimal point
				}
		++s;
		}
 	}
  // got all of mantissa - see if its a valid number, if not we are done
  if(!got_number)
 	{if(endptr!=NULL) *endptr=(char *)se;
#ifdef DEBUG
 	fprintf(stderr," strtof returns 0 (invalid number)\n"); 
#endif  	
 	 return 0;
 	}	
  if(!usingr64) r64=r32;// in this case put mantissa into both r64 and r32 so code below can use either as appropiate

  se=s; // update to reflect end of a valid mantissa
  // now see if we have an  exponent
  if(*s=='e' || *s=='E')
  	{// have exponent, optional sign is 1st
  	 ++s ; // skip 'e'
  	 if(*s=='+') ++s;
  	 else if(*s=='-') 
  	 	{expsign=true;
  	 	 ++s;
  	 	}
  	 while(isdigit(*s))
	   	{if(rexp<=2*maxfExponent)
		   rexp=rexp*10+(*s - '0');  // if statement clips at a value that will result in +/-inf but will not overflow int
		 ++s;  
		 se=s; // update to reflect end of a valid exponent (e[+-]digit+)
		}
	}
 if(endptr!=NULL) *endptr=(char *)se; // we now know the end of the number - so save it now (means we can have multiple returns going forward without having to worry about this)	
 if(expsign) rexp=-rexp;	
 rexp+=exp; // add in correct to exponent from mantissa processing
#if 1 /* if 0 removes the optimisations which just results in slower code - there is no loss of accuracy with these optimisations */
 if(!usingr64 && rexp>0 && rexp+nos_mant_digits<=9)
 	{// optimisation: can do all calculations using uint32 which is exact and fast
 	 r32*=u32powersOf10[rexp];
 	 if(sign) return -((float)r32); // negative exponent means we divide by powers of 10
 	 else return (float)r32;
 	}
 else if(!usingr64 && rexp<0 && rexp >= -7 && nos_mant_digits<=7 )
 	{// in this region we can use float rather than double as 10^6 is exact as a float (another speed optimisation, but one than thats common and therefore worthwhile)
 	 // mantissa in a float is 23 bits+ the hidden bit so 24 bits, 2^24-1 = 1.67e7 so for integers we are exact for 7 sig digits.
 	 if(sign) return -((float)r32/fltpowersOf10[-rexp]); // negative exponent means we divide by powers of 10
 	 else return (float)r32/fltpowersOf10[-rexp];
	}
#endif
 // calculate dr=(float)r*pow(10,rexp), but by using a lookup table of powers of 10 for speed and accuracy, and using doubles to ensure accuracy.
 if(rexp>0)
 	{if(rexp>maxfExponent)
		{// we have defininaly overflowed
		 if(sign) return -INFINITY;
 		 return INFINITY;
 		}
 	 if(!usingr64) 	dr=(double)r32*dblpowersOf10[rexp]; // as mantissa >= 1 this may overflow, but thats OK.
 	 else dr=(double)r64*dblpowersOf10[rexp]; // as mantissa >= 1 this may overflow, but thats OK.
	}
 else if(rexp<0)
 	{// need to take care here as mantissa is > 1 so even dividing by 10^maxExponent may not be enough, here we all division by upto 10^2*maxExponet is is by far enough
	 rexp= -rexp;
	 exp=rexp;
	 if(rexp>maxfExponent)
	 	{
 		 rexp=maxfExponent;
 		 exp-=maxfExponent; // any excess which we will also need to divide by (if its > 0)
 		}
 	  else exp=0;	
 	  if(!usingr64) dr=(double)r32/dblpowersOf10[rexp]; // negative exponent means we divide by powers of 10
 	  else dr=(double)r64/dblpowersOf10[rexp]; // negative exponent means we divide by powers of 10
 	  if(exp>0)
 	  	{dr/=dblpowersOf10[exp]; // divide by some more, we should only be dividing by max 10^18 as we only have 18 sig figs in mantissa (plus a few more if we consider creation of denormalised numbers)
 	    }
	}	
 else
 	{// special case, rexp==0
	 // do not need to use double here, so we use float for speed.
	 if(!usingr64)
	 	{ if(sign) return -((float)r32); // r is unsigned so cannot do -r !
 	 	  else return (float) r32;
	 	}
	 else
	 	{	
 	 	 if(sign) return -((float)r64); // r is unsigned so cannot do -r !
 	 	 else return (float) r64;
 	 	}
	}

 if(sign) dr= -dr;
#ifdef DEBUG
 // while this is the normal return there are several earlier return possibilities, which this will not print for (sorry).
 fprintf(stderr," strtof returns %.18g (rexp=%d, exp=%d)\n",(double)dr,rexp,exp); 
#endif 
 return (float)dr;
}
#endif 
 