
/********************************************
zmalloc.c
copyright 1991,2014-2016 Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 3, 2007.

If you import elements of this code into another product,
you agree to not name that product mawk.
********************************************/

#include  "mawk.h"
#include  "zmalloc.h"
#include <windows.h>

static void
out_of_mem(void)
{
   const char* out = "out of memory" ;

   if (mawk_state == EXECUTION)	 rt_error(out) ;
   else
   {
      /* I don't think this will ever happen */
      compile_error(out) ; mawk_exit(2) ;
   }
}

void* emalloc(size_t sz)
{
    void* ret = malloc(sz) ;
    if (ret == NULL) out_of_mem() ;
    return ret ;
}

void* erealloc(void* p, size_t sz)
{
    void* ret = realloc(p, sz) ;
    if (ret == NULL) out_of_mem() ;
    return ret ;
}

/* if we are valgrinding or purifying */

#ifdef  MEM_CHECK

void* zmalloc(size_t sz)
{
    return emalloc(sz) ;
}

void* zrealloc(void* p, size_t old, size_t new)
{
    return erealloc(p,new) ;
}

void zfree(void* p, size_t sz)
{
    free(p) ;
}

#else    /* usual  case */
/*
  zmalloc() gets mem from emalloc() in chunks of ZSIZE * AVAIL_SZ
  and cuts these blocks into smaller pieces that are multiples ZSIZE.
  When a piece is returned via zfree(), it goes
  on a linked linear list indexed by its size.	The lists are
  an array, pool[].
*/

/* block sizes are set by this #define */
#if 1
/* set block sizes based on things we are likley to allocate - ie CELL and STRING - makes 1 sec slower, try filler 8 =>28.4 secs, 16=>28.1, 32=>31.1 secs, 64=>31.3 */
/* a block size should be a power of 2 and ideally an integer multiple of max (sizeof(void *),sizeof(double)) */
typedef union zblock {
    union zblock* link ;
    double align ;
    char filler[16] ;
} ZBlock ;
#else /* original definition */
#define  ZSZ    (4*sizeof(long))

typedef union zblock {
    union zblock* link ;
    double align ;
    char filler[ZSZ] ;
} ZBlock ;
#endif

#define  ZSIZE  sizeof(ZBlock)

#define  bytes_to_blocks(b) (((b)+ZSIZE-1)/ZSIZE)

/* memory from emalloc goes here to be partitioned into
   smaller pieces  that end up in pool[]
*/

static ZBlock* avail ;
static size_t amt_avail ;

#define AVAIL_SZ    (2*1024*1024/ZSIZE)   /* number of Zblocks to get from emalloc - 2Mbytes . Coding this way allows change of filler[] size without changing number of bytes in a memory allocation */

#if 1 /* use windows virtual memory functions to directly get memory - this is slightly slower than using emalloc() ! */
static void
fill_avail(void)
{   
    void* ptr = VirtualAlloc(0, ZSIZE * AVAIL_SZ, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE); // see e.g. https://learn.microsoft.com/en-us/windows/win32/Memory/reserving-and-committing-memory 
    if (ptr == NULL) out_of_mem() ;
    avail = (ZBlock *)ptr ;
    amt_avail = AVAIL_SZ ;
}
#else
static void
fill_avail(void)
{
    avail = (ZBlock *)emalloc(ZSIZE * AVAIL_SZ) ;
    amt_avail = AVAIL_SZ ;
}
#endif

#define  POOL_SZ    32 /* was 16=>132 secs, 32=>132, 64=>133 secs */
static ZBlock* pool[POOL_SZ] ;
/* size of biggest block in pool[] */
#define zmalloc_limit  (POOL_SZ*ZSIZE) /* currently 32*16 =512 bytes with ZSIZE=16 , 16 bytes is the smallest allocation, then 32, 48,64,... 512 bytes - above that the system malloc is used */

#if 1 /* new approach, if there is not a block of the correct size then cut a larger block up */
void* zmalloc(size_t sz)
{
 if (sz > zmalloc_limit) 
 	{
     return emalloc(sz) ;
    }
 size_t  blks = bytes_to_blocks(sz) ;
 ZBlock* p = pool[blks-1] ;
 if (p) 
	{
     /* there is a block of the correct size already in the pool, just return it [ thats the fastest way to allocate memory ] */
     pool[blks-1] = p->link ;
     return p ;
	}
	
 if (blks > amt_avail) 
	{
	  /* no blocks of exact size required in pool, and not enought memory left in "avail" - look for a larger block we can cut up (this is relatively slow) */
	 for(size_t i=blks;i<POOL_SZ;++i) /* we have already checked blks-1 */
		{ZBlock* pi=pool[i];
		 if(pi)
		 	{// found some space thats larger than we need
		 	 pool[i] = pi->link ; // take 1 block of space found
		 	 p=pi; // 1st bit of space is what we return to caller
		 	 pi+=blks;// rest we put into correct free list
		 	 pi->link=pool[i-blks-1];
		 	 pool[i-blks-1]=pi;
		 	 // printf("L");
		 	 return p;
		 	}
		} 
	 /* we need some more memory (this is the slowest option) - first put any left in the correct place in pool[], then get some more memory */
     if (amt_avail > 0) 
	 	{
	 	 avail->link = pool[amt_avail-1] ;
         pool[amt_avail-1] = avail ;
    	}
     // printf("F");
     fill_avail() ;
	}
 /* cut a piece off the avail block [ this is the 2nd fastest option ] */
 p = avail ;
 avail += blks ;
 amt_avail -= blks ;
 return p ;
}
#else /* original version */
void* zmalloc(size_t sz)
{
    if (sz > zmalloc_limit) {
        return emalloc(sz) ;
    }
    {
    size_t  blks = bytes_to_blocks(sz) ;
	ZBlock* p = pool[blks-1] ;
	if (p) {
	    /* get mem from pool */
	    pool[blks-1] = p->link ;
	    return p ;
	}

	if (blks > amt_avail) {
	    if (amt_avail > 0) {
		avail->link = pool[amt_avail-1] ;
	        pool[amt_avail-1] = avail ;
	    }
	    fill_avail() ;
	}
    /* cut a piece off the avail block */
	p = avail ;
	avail += blks ;
	amt_avail -= blks ;
	return p ;
    }
}
#endif

void zfree(void* p, size_t sz)
{
    if (sz > zmalloc_limit) {
        free(p) ;
    }
    else {
	/* put p in pool[] */
        size_t blks = bytes_to_blocks(sz) ;
	ZBlock* zp = (ZBlock*) p ;
	zp->link = pool[blks-1] ;
	pool[blks-1] = zp ;
    }
}

void* zrealloc(void* p, size_t old_size, size_t new_size)
{
    if (new_size > zmalloc_limit && old_size > zmalloc_limit) {
        return erealloc(p,new_size) ;
    }
    else {
        void* ret = zmalloc(new_size) ;
        memcpy(ret, p, old_size < new_size ? old_size : new_size) ;
        zfree(p, old_size) ;
	return ret ;
    }
}

#endif
