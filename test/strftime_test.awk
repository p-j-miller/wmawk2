# strftime_test.awk: test script for strftime.awk
# code written by Peter Miller 11/9/2024
# execute as wmawk2 -f strftime.awk -f strftime_test.awk
# Assumes wmawk2 version 2.0 or later as uses systime(0) and systime(1)
#
# This version 22-5-2026 Peter Miller - edited for use in wmawk2test.bat
# Copyright (c) 2024 Peter Miller
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

BEGIN{# basic checks for strftime_awk() function
	errs=0

	printf "\nEPOCH (1 sec resolution):\n"
	t=0 # 1970-01-01 00:00:00.000000
	st=strftime_awk(t)
	print " date/time now is:",st
	t1=mktime_awk(st)
	printf(" t=%.6f\n strftime_awk() returned %s\n mktime_awk() returned %.6f\n",t,st,t1)
	if(t!=t1) {errs++;print " Error: value from systime() and from mktime_awk() should be equal"}
	if(st!="1970-01-01 00:00:00.000000") {errs++;print " Error: date-time should be 1970-01-01 00:00:00.000000"}
	
	printf "\n2024-09-11 20:07:31.021 (1ms resolution):\n"
	t=1726085251.021 # 2024-09-11 20:07:31.021
	st=strftime_awk(t)
	print " date/time now is:",st
	t1=mktime_awk(st)
	printf(" t=%.6f\n strftime_awk() returned %s\n mktime_awk() returned %.6f\n",t,st,t1)
	if(t!=t1) {errs++;print " Error: value from systime() and from mktime_awk() should be equal"}
	if(st!="2024-09-11 20:07:31.021000") {errs++;print " Error: date-time should be 2024-09-11 20:07:31.021000"}
	
	printf "\n2024-09-11 20:07:31.021234 (1us resolution):\n"
	t=1726085251.021234 # 2024-09-11 20:07:31.021234
	st=strftime_awk(t)
	print " date/time now is:",st
	t1=mktime_awk(st)
	printf(" t=%.6f\n strftime_awk() returned %s\n mktime_awk() returned %.6f\n",t,st,t1)
	if(t!=t1) {errs++;print " Error: value from systime() and from mktime_awk() should be equal"}
	if(st!="2024-09-11 20:07:31.021234") {errs++;print " Error: date-time should be 2024-09-11 20:07:31.021234"}
	printf("\n")
	if(errs==0) 
		{print "All tests passed"
		 exit(0)
		}
	else 
		{print errs,"errors found"
		 exit(1)
		}
	}
	