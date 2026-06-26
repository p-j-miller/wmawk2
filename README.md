# wmawk2

This is a port of mawk 1.9.9.6 to windows. A 64 bit windows executable is included with the release.

It was created because the version on github ( https://github.com/mikebrennan000/mawk-2 ) 
did not compile for windows when using tdm-gcc 9.2.2, and when these issues were resolved the system command did not work.

The systime function was also added as this useful and is now common in other awk implementations and this function cannot be implemented as an awk script.

As the main reason for using mawk is its speed and the tdm-gcc 9.2.2 implementation of strtod() is slow, this version also uses the much faster version from https://github.com/p-j-miller/ya-sprintf .

If you compile from source you can decide what additions are used with #defines see config.h 

wmawk2 has been heavily used and has been periodically checked against other versions of awk where it has been found to give identical answers much faster.

The wmawk2.dev file is a project file for the dev-c++ ide which is the simplest way to recompile wmawk2.

Please read the manual in the man directory which has been updated for this version and includes more information on installation and use.
## VERSIONS
Version 1.0 was the first public release. This was compiled with TDM-GCC 9.2.0, and uses MSVCRT as runtime library, which is available on all versions of Windows (but may need to be downloaded separately from Microsoft).

Version 1.1 was compiled using GCC 14.1.0 x64 (using the UCRT runtime) for Windows. The UCRT is effectively built into the operating system in Windows 10 and 11 and can be installed on earlier versions of Windows (see https://support.microsoft.com/en-gb/topic/update-for-universal-c-runtime-in-windows-c0514201-7fe6-95a3-b0a5-287930f3560c ). Some buffer sizes were also optimized for file i/o, and an internal random number generator is now used which is “more random” than the one used previously. There was no change in user functionality with 1v1, but the execution times for test programs may have changed (in some cases becoming faster).

Version 2.0 was compiled using GCC 14.2.0 x64 (using the UCRT runtime) for Windows. This version adds utf-8 functionality (” Unicode”) for the command line, environment variables, filenames and system/pipe commands and allows the console output to display uft8 characters, and utf8 characters to be input from the console via stdin. It does not change any of the string handling or matching functions [ including regular expressions] (which still assume 8-bit characters). For more details see the “Unicode support” section of the manual. Memory allocation was also changed to improve its speed and, in some conditions, it will use (potentially a lot) less memory, while in other situations it may use a little more memory than previously. Hexadecimal constants (staring 0X or 0X) are now allowed in awk scripts. Systime(0) and systime(1) functions added.

Version 2.1 was compiled using GCC 15.2.0 x64 (using the UCRT runtime) for Windows for 
a processor supporting at least SSE4.2 (this is a requirement for running under Windows 
11, and processors supporting this have been available since 2008). The –version option 
now also displays a summary of the processor’s instruction set (SSE, AVX), but not having 
SSE4.2 will give an error message when Wmawk2 is started. Note that at the time version 
2.1 was released gcc 16.1.0 had just become available, however while the wmawk2 code 
compiles cleanly with this version the execution speed was a little slower on all the tests 
tried by the author so the decision was taken to stick with 15.2.0.

No new functionality was added, but floating point output is now much faster (over 8
times faster in some cases) by using the latest release of ya_sprintf() ( 
https://github.com/p-j-miller/ya-sprintf ) which uses the fpfmt algorithm. This is believed 
to be the fastest algorithm at present for floating point conversion. In some common cases 
(cast of integers and floating-point numbers to strings, print() for integers and floating-point numbers and printf() & sprintf() with formats %e, %g, %n, %u) the conversion is 
now done inline in wmawk2 by directly calling the low-level functions within 
ya_sprintf so avoiding the overhead of parsing the format string at runtime, this means 
some integer output functions are over 2* faster. Only integers in the range -
10000000<i<+100000000 are printed quickly (up to 7 digits for negative numbers and 8 
digits for positive numbers), and only basic “printf” formats are quickly processed (“%n”, 
“%u”, “%e”, “%.Pe”, “%g”, “%.Pf” where P is an integer (the precision) ). These can be 
combined, for example in the format “%u: %i, %.5g, %10e\n” all the integer (assuming the 
values where in the range specified above) and floating-point conversions would be sped 
up by being directly implemented in Wmawk2. These speed ups are completely 
transparent to the user, as the output produced is identical. Note however that Wmawk 
2v1 can produce slightly different numerical values at 15 significant digits and above. For 
17 significant digits and above wmawk2 creates numbers that are “round-the-loop” exact 
meaning that if the number is read back in it will create exactly the same internal value.
Note there are a lot of different numbers that have this property for the same internal value 
– for example for one particular internal value to 22 significant digits v2.0 gives “-
5.357105737088430404663e+09” while 2v1 gives “-5.357105737088430400000e+09” [ 
the first 18 significant digits are identical here]. The use of trailing zero’s in 2v1 does have a 
side effect if the %g format is used, as %g removes trailing zero’s, in the above example this 
gives for 2v0 “-5357105737.088430404663”, while 2v1 gives “-5357105737.0884304” as 
its trailing zeroes are removed (all these numbers are “round-the-loop” exact as the first 18 
significant digits are identical).
For less than 15 significant digits there is only 1 correctly rounded value which wmawk2 
will create. For 15 and 16 significant digits there can be more than one correctly rounded 
value so its possible different wmawk2 versions will create (very) slightly different 
numbers for this range.

The random number generator (rand() function) was also updated and now generates 
different, but more random, numbers. Note that the AWK-library at https://github.com/p-j-miller/AWK_library provides a portable (written in awk) random number generator that 
should provide the same pseudo random sequence on any (wm)awk interpreter. This 
library also provides (wm)awk functions for very fast sorting, median calculation, and 
conversion between strings and times as seconds since the EPOCH (as returned by 
systime()). 

The test programs in the test subdirectory were also updated and now test every wmawk2 
keyword and function as well as providing enhanced “stress tests” for things like memory 
management. A batch file was also created to execute them all and monitor the results –
which is called wmawk2test.bat . This takes one argument – the name of the wmawk2 
executable (with full path if it’s not on the command search path). On a Windows 11 
system based on an i3-10100 processor the total execution time of all the test programs for 
2v0 was 224.049 seconds while 2v1 took 124.294 seconds giving an average speed up of 
1.8*. All of the 3 tests that are individually timed were also faster with 2v1. Apart from the 
execution time, the results are identical for 2v0 and 2v1. Note that about 1.5GB of free disk 
space is required to run these test programs as handling of large files is part of the testing 
conducted. 

Note that it was not possible to upload the file print_test.out which should be in the test subdirectory as its ~ 1.4GB and github's limit is 25MB. 
It can be created with the command below (which should only need to be done once):
~~~
 wmawk2 -f print_test.awk >print_test.out
~~~
Version 2.2 adds %f to the list of printf() formats that are optimized (which gives a small speed improvement with the test programs). It also swaps to the latest version of ya_sprintf (2v4) which resolves an issue that can result in incorrect rounding with %f formats (giving a result potentially in error by one in the least significant digit). This means the file print_test.out also needs to be updated (see instructions above).

Version 2.3 swaps to the latest version of ya_sprintf (2v5) which is faster for some numbers when converted via %f. The example below (crafted to give the maximum improvement) runs ~ 10% faster with wmawk2 version 2.3, but no significant speedup is shown on the test programs in the directory “test” when running wmawk2test.bat.
~~~
wmawk2 "BEGIN{s=systime(1);for(i=0;i<1e9;++i) z=sprintf(\"%.1f\",0.25);print systime(1)-s}"
~~~
For comparison wmawk2 2v2 is 25% faster than 2v1 on this test.


