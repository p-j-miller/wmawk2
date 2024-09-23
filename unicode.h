/* unicode.h
   =========
 Basic unicode file handling functions for C console programs using mingw with ucrt
 Has the following functions:
   char * wide_to_utf8(wchar_t *wsource) - converts wchar_t string to uft8. you MUST free the returned string! Returns NULL on error
   wchar_t *utf8_to_wide(const char *usource) - converts utf8 string to wchar_t. you MUST free the returned string! Returns NULL on error
   int setconsoleout_utf8(void) - sets console so it displays utf8 characters correctly , returns 0=bad, 1=ok. Must be called before uisng stdout/stderr
   int umain(int argc,char **argv,char **enpv) - call user code , use in place of main(), "converts" argv to pass utf8 text from the command line & converts enpv to utf8
It also redefines the following functions to suppport utf8 strings:
   FILE *fopen(char *u8filename,char *umode) - like fopen but allowing uft8 filenames
   int open( const char *u8filename, int oflag [,int pmode]) - like open but allowing uft8 filenames
   FILE *popen(const char *u8command, const char *umode) -  like popen but allowing uft8 filenames
   int system(const char *u8command) -  like system but allowing uft8 filenames
   char *fgets( char *str,int numChars,FILE *stream); - like fgets but allows utf8 characters
   
 This is windows specific code - only tested in mingw gcc14.1.0 w64 with UCRT - this means these files only define things if __MINGW32__ is defined (except for umain()) 

Warning: even though it redefines the standard names, "unicode.h" needs to be included in all relevant files as some things in stdio.h are macros (in particular for popen() )!

 Written by Peter Miller
   
 Versions
 1.1 22/7/2024
 1.2 23/7/2024 - adds usystem()
 1.3 23/7/2024 - 1st full version with header file etc
 1.4 23/7/2024 - added umain()
 1.5 23/7/2024 - added envp param to main()
 2.0 24/7/2024 - 1st version that actually redefines core functions to make their use transparent
 2.1 25/7/2024 - sets console text to UTF-8 before calling umain (so user does not need to call setconsoleout_utf8()
 2.2 26/7/2024 - fgets() added and stdin set to correct mode before calling umain() 
2.3 4/8/2024  - use fputs() rather than puts() in test code below for pipes to avoid extra newlines being inserted in output
 2.4 5/8/2024  - when opening FILE * (fopen or popen) for reading need to use _setmode() to correctly read utf8 characters
*/
/*----------------------------------------------------------------------------
 * Copyright (c) 2024 Peter Miller
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
#if !defined( __UNICODE_H) && defined(__MINGW32__)
#include <wchar.h> /* needed for wchar_t */
#include <stdio.h> /* needed for FILE * */

 #define __UNICODE_H
 #ifdef __cplusplus
  extern "C" {
 #endif
	char *wide_to_utf8(wchar_t *wsource); /* converts wchar_t string to uft8. you MUST free the returned string! Returns NULL on error */
	wchar_t *utf8_to_wide(const char *usource); /* converts utf8 string to wchar_t. you MUST free the returned string! Returns NULL on error */
	FILE *fopen(const char *u8filename,const char *umode); /* like fopen but allowing uft8 filenames */
	int open( const char *u8filename, int oflag ,...); /* like open but allowing uft8 filenames */
#ifdef popen
 #undef popen /* mingw gcc defines this as _popen() which causes compilation warnings... */
#endif
	FILE *popen(const char *u8command, const char *umode); /* like popen but allowing uft8 filenames */
	int system(const char *u8command); /*  like system but allowing uft8 filenames */
	char *fgets( char *str,int numChars,FILE *stream); /* like fgets() but allows utf8 characters to be read in */
	int setconsoleout_utf8(void); /* sets console so it displays utf8 characters correctly , returns 0=bad, 1=ok */
  #if 1 /* support envp for umain */ 
  	int umain(int argc,char **argv,char **enpv);/* call user code , use in place of main(), "converts" argv to pass utf8 text from the command line & converts enpv to utf8  */
  #else
	int umain (int argc, char **argv); /* call user code , use in place of main(), "converts" argv  to pass utf8 text from the command line  */
  #endif
 #ifdef __cplusplus
    }
 #endif
#endif	
