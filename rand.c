/* replacement for library rand() function that has known (good) properties
   The default generator in tdm-gcc 10.3.0 has RAND_MAX = 32767 which is not ideal.

Public domain code for JKISS RNG   from   http://www0.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf
claims any of the 2 generators combined still pass all the Dieharder tests (and obviously all 3 combined do).
It also passes the complete BigCrunch test set in testU01.
Its period is ~ 2^127 (1.7e38).
Initial constants , srand() etc by Peter Miller 24/11/2023

WARNING: rand.h must be included in any file using these functions (if not they could be called as ya_rand(), but RAND_MAX is likley to be wrong!)
If used for crytographic applications see warning on srand() below.
*/
#include <stdint.h>
#include "rand.h"
// max 32 bit unsigned is 4,294,967,295 (2^32-1)
// constants here are all prime numbers from https://en.wikipedia.org/wiki/List_of_prime_numbers 
// 433494437 = 433,494,437
// 2971215073 = 2,971,215,073 this  has msb set as > 2^31 (see below)
// 2147483647 = 2,147,483,647 = 2^31-1
// 2521008887 = 2,521,008,887 this also has msb set
static uint32_t x=433494437U,y=2971215073U,z=2147483647U,c=2521008887U;/* Seed variables */
static uint32_t JKISS(void)
{
 uint64_t t;
 x=314527869*x+1234567;
 y^=y<<5;
 y^=y>>7;
 y^=y<<22;
 t =4294584393UL*z+c;
 c= t>>32;
 z= t;
 return x+y+z;
}

int ya_rand(void)
{return JKISS() & RAND_MAX; // RAND_MAX is 2^31-1 (0x7fffffff ) so always 32 bit (and positive) even if int is 64 bit 
}

void ya_srand(unsigned int seed)
{ // seed random number generator, 0 gives the same sequence as you get without calling srand() 
  // warning - this will only give a number of different sequences limited by the size of an unsigned int - ideally the seed needs to be 4 * 32 bit unisgned numbers but this matches the standard C library srand().
 if(seed==0)
 	{x=433494437U; // same as initial values 
	 y=2971215073U;
	 z=2147483647U;
	 c=2521008887U;
 	}
 else
 	{// use various manipulations of seed to set x,y,z,c 
	 x=433494437U^(~seed);
	 y=2971215073U+(~seed*257U);// *prime number
	 z=2147483647U^(seed*65537U);// *prime number
	 c=2521008887U+(seed*1442968193U);// *prime number 1442968193 = 1,442,968,193
 	}
}

