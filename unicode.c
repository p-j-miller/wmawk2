/* unicode.c
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
 
 This is windows specific code - only tested in mingw gcc14.1.0 w64 with UCRT.
 if not compiled under mingw then only creates code to call umain()
 
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
 2v5 6/8
 2v6 6/8
 
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

#ifdef __MINGW32__ /* this code has only be tested with mingw - in fact only with gcc14-1-0-w64-ucrt */
#include <windows.h> // for MultiByteToWideChar(), CP_UTF8, etc.
#include <stdarg.h> // popen() has a variable number of arguments
#include <fcntl.h> /* needed for _O_CREAT */
#include "unicode.h"
#include <stdbool.h>

static int dowildcard=1 ; /* for umain below - if 1 do command line expansion , if 0 do not */

/* unicode support test program needs -municode adding to compiler and linker */
// see https://sourceforge.net/p/mingw-w64/wiki2/Unicode%20apps/ & https://github.com/coderforlife/mingw-unicode-main/blob/master/mingw-unicode.c
// see also https://learn.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page



char * wide_to_utf8(wchar_t *wsource)  /* convert wchar_t string to uft8. you MUST free the returned string! Returns NULL on error*/
{
 size_t required_size = WideCharToMultiByte(CP_UTF8, 0, wsource, -1, NULL, 0, NULL, NULL); // find length needed
 if(required_size==0) return NULL;
 char *buffer = (char *) calloc(required_size, sizeof(char));
 if(buffer==NULL) return NULL;
 if(WideCharToMultiByte(CP_UTF8, 0, wsource, -1, buffer, required_size, NULL, NULL)!=required_size) // actually do conversion
  	{ free(buffer);
	  return NULL;
	}
 return buffer;
}

wchar_t *utf8_to_wide(const char *usource) /* convert utf8 string to wchar_t. you MUST free the returned string! Returns NULL on error*/
{
 size_t required_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, usource, -1, NULL, 0); // find length needed
 if(required_size==0) return NULL;
 wchar_t *buffer = (wchar_t *) calloc(required_size, sizeof(wchar_t));
 if(buffer==NULL) return NULL;
 if(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, usource, -1, buffer, required_size)!=required_size)// actually do conversion
 	{free(buffer);
	 return NULL;
    }
 return buffer;
}

FILE *fopen(const char *u8filename,const char *umode)
{// like fopen but allowing uft8 filenames
 // 1st convert filename to wide
 wchar_t *wfilename,*wmode;
 wfilename=utf8_to_wide(u8filename);
 if(wfilename==NULL) return NULL; // conversion failed
 // now convert mode to wide
 wmode=utf8_to_wide(umode);
 if(wmode==NULL) 
 	{free(wfilename);
	 return NULL; // conversion failed 
	}
 FILE *fo= _wfopen(wfilename, wmode);
#if 1
 bool isread=false;
 for(const char *s=umode;*s;++s)
 	if(*s=='r') isread=true;
 if(isread)
 	_setmode( _fileno( fo ), _O_U8TEXT ); /* set to utf8, only for reading */
#endif 	
 free(wfilename);
 free(wmode);
 return fo;
}

/* like above for open():
	int _open( const char *filename, int oflag [,int pmode]);
	int _wopen( const wchar_t *filename, int oflag [, int pmode]);
*/
int open( const char *u8filename, int oflag ,...)
{// like open but allowing uft8 filenames
 // 1st convert filename to wide
 // note pmode is "optional" which makes the code more complex
 // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/open-wopen?view=msvc-170 describes the functionality
 // see also https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
 wchar_t* wfilename; 
 int pmode=0;
 va_list ap;
 wfilename=utf8_to_wide(u8filename);
 if(wfilename==NULL) return -1;// conversion failed
 int fo;
 if(oflag & _O_CREAT )
 	{// pmode must be specified for _O_CREAT (only)
 	 va_start(ap, oflag);
 	 pmode=va_arg(ap, int); // get pmode
 	 va_end(ap); // must be called before open() terminates (once we have called va_start() )
 	}	
 fo= _wopen(wfilename, oflag,pmode);
 free(wfilename);
 return fo;
}

/*
  Like above but for popen()
	FILE *_popen(const char *command,const char *mode);
	FILE *_wpopen(const wchar_t *command,const wchar_t *mode);
*/
FILE *popen(const char *u8command, const char *umode)
{// like popen but allowing uft8 filenames
 // 1st convert command to wide
 wchar_t *wcommand,*wmode;
 wcommand=utf8_to_wide(u8command);
 if(wcommand==NULL) return NULL; // conversion failed
 // now convert mode to wide
 wmode=utf8_to_wide(umode);
 if(wmode==NULL) 
 	{free(wcommand);
	 return NULL; // conversion failed 
	}
  FILE *fo= _wpopen(wcommand, wmode);
#if 0  /* this is not needed for wmawk2 */
  bool isread=false;
  for(const char *s=umode;*s;++s)
 	if(*s=='r') isread=true;
  if(isread)
 	_setmode( _fileno( fo ), _O_U8TEXT ); /* set to utf8, only for reading from pipe*/
#endif
#if 0 	
  else 
    _setmode( _fileno( fo ), _O_WTEXT ); /* writing to pipe is also utf8 */
#endif
  free(wcommand);
  free(wmode);
  return fo;
}

/* system() - same as system(), but accepts a utf8 encoded string 
	int system(const char *command);
	int _wsystem(const wchar_t *command);
*/
int system(const char *u8command)
{ wchar_t *wcommand;
 wcommand=utf8_to_wide(u8command);
 if(wcommand==NULL) return -1; // conversion failed
 int r=_wsystem(wcommand);
 free(wcommand);
 return r;
}

/* fgets replacement that reads utf8 character strings reads till specified number of chars (bytes)-1 (to allow for terminating 0)  or EOF or \n 
 returns NULL on eof or error, otherwise returns str.
  Note this function may return less than the requested number of characters , and wil always give an error if less than 5 characters are specified [ie 5 is the smallest valid number]
  as a single utf8 character may take 4 bytes and there needs to be space for the terminating 0 character.
 
	char *fgets( char *str,int numChars,FILE *stream);
	wchar_t *fgetws(wchar_t *str,int numChars,FILE *stream);
*/
#if 1 /* a line like 123<control>Z<ENTER> returns 123 then NULL (to signal EOF) - note there is no newline in this case */
char *fgets( char *str,int numChars,FILE *stream)
{
 *str=0;// just in case
 if(numChars< 5) return NULL;// invalid param
 if(feof(stream) || ferror(stream)) return NULL;
 wchar_t *wbuf=(wchar_t *) calloc(numChars+1, sizeof(wchar_t));// cannot be more wide chars than chars
 wchar_t *r;
 if(wbuf==NULL) return NULL;// calloc() failed
 r=fgetws(wbuf,(numChars)/4+1,stream);// each utf8 character is a maximum of 4 bytes, fgetws() guarantees a terminating 0 character (allowed for in the "+1" for the number of chars specified)
 if(r==NULL)
 	{free(wbuf);
	 return NULL;
	}
 WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, str, numChars, NULL, NULL); //do conversion to uft8 - fgetws should always return a valid string so conversion should suceed 
 free(wbuf);
 return str;
}
#else /* EOF anywhere in a line gives EOF  with any characters before the EOF lost */
char *fgets( char *str,int numChars,FILE *stream)
{
 *str=0;// just in case
 if(numChars<5) return NULL;// invalid param
 wchar_t *wbuf=(wchar_t *) calloc(numChars+1, sizeof(wchar_t));// cannot be more wide chars than chars
 wchar_t *r;
 if(wbuf==NULL) return NULL;// calloc() failed
 r=fgetws(wbuf,(numChars)/4+1,stream);// each utf8 character is a maximum of 4 bytes, fgetws() guarantees a terminating 0 character
 if(r==NULL || feof(stream) || ferror(stream))
 	{free(wbuf);
	 return NULL;
	}
 WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, str, numChars, NULL, NULL); //do conversion to uft8 - fgetws should always return a valid string so conversion should suceed 
 free(wbuf);
 return str;
}
#endif

int setconsoleout_utf8(void) /* - sets console so it displays utf8 characters correctly , returns 0=bad, 1=ok */
{
 /* this sets char output to be uft8 unicode - wide char output is not changed (so is NOT unicode!). */
 if(IsValidCodePage(CP_UTF8))
  	{
      SetConsoleOutputCP(CP_UTF8);// see https://stackoverflow.com/questions/46512441/how-do-i-print-unicode-to-the-output-console-in-c-with-visual-studio
      return 1; /* sucess */
	}
 return 0; /* failed */	
}


/* see https://learn.microsoft.com/en-us/cpp/c-runtime-library/getmainargs-wgetmainargs?view=msvc-170 & 
   https://github.com/coderforlife/mingw-unicode-main/blob/master/mingw-unicode.c
	int __wgetmainargs (
	   int *argc,
	   wchar_t ***argv,
	   wchar_t ***env,
	   int doWildCard,
	   _startupinfo * startInfo)
*/
void __wgetmainargs(int*,wchar_t***,wchar_t***,int,int*); /* not in mingw includes ?  */
int main() 
{
 wchar_t **wenpv, **wargv;
 int argc, si = 0;// &si is a pointer to a structure, whose 1st element (si here) is its length (0 here) - see https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfow
 __wgetmainargs(&argc, &wargv, &wenpv, dowildcard, &si); // this also creates the global variable __wargv
 /* create argv and call umain() */
 // now recreate char **argv
 char **argv= (char **) calloc(argc, sizeof(char*));
 for(int i=0;i<argc;++i)
 	{argv[i]=wide_to_utf8(wargv[i]);
 	}
 setconsoleout_utf8(); /* set console output (stdout, stderr) to support uft-8 characters */

 if(_isatty(_fileno( stdin )))
 	_setmode( _fileno( stdin ), _O_WTEXT ); /* stdin is also utf8 (only needed if interactive input) */

#if 1
 /* array of environment variables is terminated with a NULL pointer, convert these to UTF8 strings */
 size_t count_env=0;
 while(wenpv[count_env]!=NULL) ++count_env;// find out how many environment variables there are

 char ** enpv=calloc(count_env+1, sizeof(char*)); // need 1 more for final NULL
 size_t ei;
 for(ei=0;ei<count_env;++ei)
 	enpv[ei]=wide_to_utf8(wenpv[ei]);// convert all to utf8
 enpv[ei]=NULL;// add in final null to terminate array
 return umain(argc, argv, enpv);
#else /* no envp */
 return umain(argc, argv);
#endif
}
#else
/* if not under mingw then only add code to call umain() */
int main (int argc, char **argv,char **enpv)
 {return umain(argc, argv,enpv);
 }
#endif
