/* header file for rand.c
If this header file is not included RAND_MAX wil be the compiler default, as will rand() and srand()
*/
#ifdef RAND_MAX
 #undef RAND_MAX
#endif
#define RAND_MAX 0x7fffffff /* in case its used (2^31-1)*/
#define rand() ya_rand()
#define srand(x) ya_srand(x)
#ifndef __RAND_H__
#define __RAND_H__
 #ifdef __cplusplus
extern "C" 
 {
 #endif
  int ya_rand(void);
  void ya_srand(unsigned int seed);
 #ifdef __cplusplus
 }
 #endif
#endif