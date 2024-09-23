# wmawk2

This is a port of mawk 1.9.9.6 to windows. A 64 bit windows executable is included with the release.

It was created because the version on github ( https://github.com/mikebrennan000/mawk-2 ) 
did not compile for windows when using tdm-gcc 9.2.2, and when these issues were resolved the system command did not work.

The systime function was also added as this useful and is now common in other awk implementations and this function cannot be implemented as an awk script.

As the main reason for using mawk is its speed and the tdm-gcc 9.2.2 implementation of strtod() is slow this version also uses the much faster version from https://github.com/p-j-miller/ya-sprintf .

If you compile from source you can decide what additions are used with #defines see config.h 

wmawk2 has been heavily used and has been periodically checked against other versions of awk where it has been found to give identical answers much faster.

The wmawk2.dev file is a project file for the dev-c++ ide which is the simplest way to recompile wmawk2.

Please read the manual in the man directory which has been updated for this version and includes more information on installation and use.
## VERSIONS
Version 1.0 was the first public release. This was compiled with TDM-GCC 9.2.0, and uses MSVCRT as runtime library, which is available on all versions of Windows (but may need to be downloaded separately from Microsoft).

Version 1.1 was compiled using GCC 14.1.0 x64 (using the UCRT runtime) for Windows. The UCRT is effectively built into the operating system in Windows 10 and 11 and can be installed on earlier versions of Windows (see https://support.microsoft.com/en-gb/topic/update-for-universal-c-runtime-in-windows-c0514201-7fe6-95a3-b0a5-287930f3560c ). Some buffer sizes were also optimized for file i/o, and an internal random number generator is now used which is “more random” than the one used previously. There was no change in user functionality with 1v1, but the execution times for test programs may have changed (in some cases becoming faster).
