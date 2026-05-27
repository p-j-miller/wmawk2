# test of misc functions not covered in other tests
# Created by Peter Miller 24-5-2026
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
#
BEGIN{
	  CONVFMT="%.10g" # change from default (%.6g) as we use casting of number to string to avoid issues with very small differences. We also use steps of 0.0078125 which caused issues with %.6g due to the final 5 making the default rounding ineffective in some cases
		# as a useful side effect also checks CONVFMT keyword
	  # Start with atan2(y,x)=arctan of y/x between -pi and +pi
	  # we known arctan(1)+arctan(2)+arctan(3) should equal pi - https://en.wikipedia.org/wiki/List_of_trigonometric_identities
	  print "Checking arctan(1)+arctan(2)+arctan(3) equals pi:"
	  api=atan2(3,3)+atan2(6,3)+atan2(9,3)
	  if(api<3.1415926 || api> 3.1415927) # PI=3.14159265....
		{++errs;
		}
	  printf("  arctan(1)+arctan(2)+arctan(3)=%.18g should = PI\n",api);
	  # check sin & cos using identify sin(x)^2+cos(x)^2=1
	  # and arctan(tan(x))=x with tan(x)=sin(x)/cos(x) - this checks both arguments of atan2(a,b)
	  # 0.0078125 = 1/128 which can be represented exactly as a floating point number, pi is calculated above
	  print "Checking sin(x)^2+cos(x)^2=1 and arctan(tan(x))=x:"
	  for(x=0;x<2*api;x+=0.0078125)
		{if(x==0) continue # checks continue operation which is not used otherwise
		 sin_x=sin(x)
		 cos_x=cos(x)
		 if(cos_x==0) continue # avoids 1/0 in code below
		 atan2_x=atan2(sin_x,cos_x)
		 if(atan2_x<0) atan2_x=2*api+atan2_x
		 printf(" %g: sin(%g)=%g cos(%g)=%g sin^2+cos^2=%g atan(tan(x))=%g\n",x,x,sin_x,x,cos_x,sin_x*sin_x+cos_x*cos_x,atan2_x)
		 if((sin_x*sin_x+cos_x*cos_x) "" !="1") 
			{++errs # cast to string as checking as double precision might show up very small differences
			 printf("  error: sin^2+cos^2=%g at x=%g\n",sin_x*sin_x+cos_x*cos_x,x)
			}
		 if(atan2_x "" != x "") 
			{++errs
			 printf("  error: arctan(tan(x))=%.18g while x=%.18g\n",atan2_x,x)
			}
		}
	  printf("%d errors so far\n",errs)

	  # now check using identity sin(arctan(x))=x/sqrt(1+x^2)  - https://en.wikipedia.org/wiki/List_of_trigonometric_identities#Inverse_trigonometric_functions  
	  print "Checking sin(arctan(x))=x/sqrt(1+x^2):"
	  for(x=-100;x<100;x+=1)
		{
		 sin_atan=sin(atan2(x,1))
		 x_sqrt=x/sqrt(1+x*x)
		 printf(" x=%g: sin(arctan(x))=%g x/sqrt(1+x^2)=%g\n",x,sin_atan,x_sqrt)
		 if((sin_atan "") != (x_sqrt "")) 
			{++errs # cast to string as checking as double precision might show up very small differences
			 printf("  error %d: %s != %s\n",errs,(sin_atan ""),(x_sqrt ""))
			}
		}
	  printf("%d errors so far\n",errs)
	  # now check random number generator
	  srand(5) # force to start at the same value every time
	  avg_count=1000000
	  for(i=1;i<=avg_count;++i)
		avg+=(rand()-avg)/i # starting at i=1 means this initialises correctly
	  diff=avg-0.5
	  if(diff<0) diff=-diff
	  if(diff>0.5/sqrt(avg_count)) 
		{++errs
		 printf("srand(5): Average of %u random numbers is %g (should be close to 0.5)\n",avg_count,avg)
		 printf(" expected difference from 0.5 is <=%g actual difference is %g : %s\n",0.5/sqrt(avg_count),diff,diff<=0.5/sqrt(avg_count)?"OK":"Suspect")
		}
	  else print "srand(5) test OK"
	  # repeat with a different starting value for the random sequence
	  srand(10)
	  for(i=1;i<=avg_count;++i)
		avg+=(rand()-avg)/i # starting at i=1 means this initialises correctly
	  diff=avg-0.5
	  if(diff<0) diff=-diff
	  if(diff>0.5/sqrt(avg_count)) 
		{++errs
		 printf("srand(10): Average of %u random numbers is %g (should be close to 0.5)\n",avg_count,avg)
		 printf(" expected difference from 0.5 is <=%g actual difference is %g : %s\n",0.5/sqrt(avg_count),diff,diff<=0.5/sqrt(avg_count)?"OK":"Suspect")
		}
	  else print "srand(10) test OK"
	  # now test log() and exp() use identity exp(0.5*ln(x)) = sqrt(x)
	  print "now testing log() and exp() using identity exp(0.5*ln(x)) = sqrt(x)"
	  for(x=1;x<=16;x+=0.25)
		{e_l=exp(0.5*log(x))
		 s=sqrt(x)
		 printf(" x=%g: exp(0.5*ln(x))=%g sqrt(x)=%g\n",x,e_l,s)
		 if(e_l "" != s "") ++errs
		}
	  # index, tolower,toupper and fflush to go...
	  Print "Testing index() function"
	  string1="hello World"
	  string2="World"
	  r=index(string1,string2) 
	  string3=substr(string1,r,length(string2))
	  if(string2!=string3) 
		{++errs
		 printf(" test of index() failed - string2=%s not equal to string3=%s\n",string2,string3)
		}
	  else printf(" test of index() function passed - r=%d string3=%s\n",r,string3)
	  # check tolower()
	  string1=tolower(string1)
	  string2="lo w"
	  r=index(string1,string2) 
	  string3=substr(string1,r,length(string2))
	  if(string2!=string3) 
		{++errs
		 printf(" test of index() & tolower() functions failed - string2=%s not equal to string3=%s\n",string2,string3)
		}
	  else printf(" test of index() & tolower() functions passed - r=%d string3=%s\n",r,string3)
	  # check toupper()
	  string1=toupper(string1)
	  string2="WOR"
	  r=index(string1,string2) 
	  string3=substr(string1,r,length(string2))
	  if(string2!=string3) 
		{++errs
		 printf(" test of index() & toupper() functions failed - string2=%s not equal to string3=%s\n",string2,string3)
		}
	  else printf(" test of index() & toupper() functions passed - r=%d string3=%s\n",r,string3)
	  # now test fflush()
	  print string1 >"test1.out"
	  r=fflush("test1.out") # without this call to fflush() the getline below fails to read string1 back again proving it does work as expected
	  if(r!=0) 
		{++errs
		 printf("Error: fflush() did not return 0 - returned %d\n",r)
		}
	  getline string1a < "test1.out"
	  if(string1!=string1a) 
		{++errs
		 printf("Error: reading from file after fflush() - string1=%s, string1a=%s\n",string1,string1a)
		}	 
	  r=close("test1.out")
	  if(r!=0) 
		{++errs
		 printf("Error: close() did not return 0 - returned %d\n",r)
		}
	  printf(" misc functions test finished %d errors found\n",errs)
	  printf(" misc functions test finished %d errors found\n",errs) >"/dev/stderr"
	  if(errs>0) exit(1)
	  else exit(0)
	}