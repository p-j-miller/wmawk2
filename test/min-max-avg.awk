	# Written by Peter Miller 7/5/2026
	# this version for use in wmawk2test.bat only checks values that should be accurate between wmawk2 versions
	
	# Copyright (c) 2026 Peter Miller
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
	
BEGIN{# calculate min, max,average of column 6 (other columns are ignored)
	 start_t=systime(1) # if you are not using wmawk , then delete the "1", but the timing resolution may then only be 1 sec
	 col=6 # a column in file print_test.out that has a wide range of different values 
	 }
	{val=$(col)+0 # cast to number as not all lines in file have valid numbers in col 6
	 if(NR==1)
		{min=val
		 max=val
		 avg=val
		}
	 else
		{if(val<min) min=val
		 if(val>max) max=val
		 avg+=(val/NR-avg/NR) # avg+=(val-avg)/NR gives -nan, new formulation gives   min=-1.797693e+308 average=4.616089452e+18 max=1.797693e+308
		}
	}
END {end_t=systime(1) # if you are not using wmawk , then delete the "1", but the timing resolution may then only be 1 sec
	 printf("%d lines read, for column %d min=%.10g average=%.10g max=%.10g\n",NR,col,min,avg,max)
	 printf(" execution time was %g secs\n",end_t-start_t) >"/dev/stderr"
	}
	