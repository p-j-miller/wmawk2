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
