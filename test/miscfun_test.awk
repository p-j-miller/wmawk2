# test of misc functions not covered in other tests
# Created by Peter Miller 24-5-2026
	# this version 26/9/2026  for use in wmawk2test.bat checks values to full resolution via "prints" and maximum possible resolution without causing false errors internally. It therefore only passes when "cr_xxx" maths functions are used.
	# it also adds timing for sqrt(),log(),exp(),^,sin,cos and atan2 functions (both making their contribution to the total "significant" and giving ns/call values to the screen). 
	
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
	
# example results on i3-10100
# -m32
# testing misc functions not already tested
#  Timing for maths functions:
#   Baseline - no function in loop : sum=3290235273891.77832 time=4.75 secs
#   sqrt(x): sum=27452696229.5771446 time for sqrt=0.71 secs (2.8 ns/call)
#   log(x): sum=2357302309.59457541 time for log()=2.71 secs (10.5 ns/call)
#   exp(x): sum=5.85918444506039478e+289 time for exp()=3.67 secs (13.9 ns/call)
#   power(x,0.5): sum=2255391705.90056992 time for power()=8.87 secs (67.1 ns/call)
#   sin() & cos() sum=-161698.767145868536 time for sin+cos=12.13 secs (18.8 ns/call to either sin() or cos())
#   atan2() sum=-7.09651165427377961 time=9.33 secs (28.9 ns/call)
#  misc functions test finished in 83.05 secs , 0 errors found
#  test of misc functions not already tested passed
# 
# 
# -m64
# testing misc functions not already tested
#  Timing for maths functions:
#   Baseline - no function in loop : sum=3290235273891.77832 time=5.35 secs
#   sqrt(x): sum=27452696229.5771446 time for sqrt=0.76 secs (3.0 ns/call)
#   log(x): sum=2357302309.59457541 time for log()=1.71 secs (6.6 ns/call)
#   exp(x): sum=5.85918444506039478e+289 time for exp()=1.39 secs (5.3 ns/call)
#   power(x,0.5): sum=2255391705.90056992 time for power()=3.69 secs (27.9 ns/call)
#   sin() & cos() sum=-161698.767145868536 time for sin+cos=9.51 secs (14.7 ns/call to either sin() or cos())
#   atan2() sum=-7.09651165427377961 time=6.94 secs (21.5 ns/call)
#  misc functions test finished in 71.25 secs , 0 errors found
#  test of misc functions not already tested passed
# 
# 
#
BEGIN{
	   CONVFMT="%.12g" # change from default (%.6g) as we use casting of number to string to avoid issues with very small differences. %.12g is the largest possible without giving errors
	   start_test=systime(1)
		# as a useful side effect also checks CONVFMT keyword
		# we print values to file (via stdout) as %.20g and then do a charcater by character comparison in the batch file so we do check results are bitwise identical
		
		# Without using the cr_xxx functions there are issues, in particular the issue occurs with x=0.7421875 and gcc 16.2.0 using the "winlib" port
		# gcc -m32 gives:
		 #wmawk2 "BEGIN{x=0.7421875;printf(\"%.20g %.20g %.20g %.20g\n\",x,sin(x),cos(x),x-atan2(sin(x),cos(x)))}"
		 # 0.7421875 0.67590169702617886 0.736991788256240787 0
	   # while gcc -m64 gives:
	    #wmawk2 "BEGIN{x=0.7421875;printf(\"%.20g %.20g %.20g %.20g\n\",x,sin(x),cos(x),x-atan2(sin(x),cos(x)))}"
	    #0.7421875 0.675901697026178749 0.736991788256240787 1.11022302462515654e-16
		# so without using the cr_xxx functions, the 64 bit compiler introduces a 1 bit error (in sin(x) )which is not present with the 32 bit compiler! (printing the difference from x shows this is real and not an issue with printing floating point values)
		# with the cr_xxx functions, both give the same result (zero error as -m32 above).
		
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
		 #printf(" %g: sin(%g)=%g cos(%g)=%g sin^2+cos^2=%g atan(tan(x))=%g\n",x,x,sin_x,x,cos_x,sin_x*sin_x+cos_x*cos_x,atan2_x)
		 printf(" %.20g: sin(%.20g)=%.20g cos(%.20g)=%.20g sin^2+cos^2=%.20g atan(tan(x))=%.20g\n",x,x,sin_x,x,cos_x,sin_x*sin_x+cos_x*cos_x,atan2_x)
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
		 printf(" x=%g: sin(arctan(x))=%.20g x/sqrt(1+x^2)=%.20g\n",x,sin_atan,x_sqrt)
		 if((sin_atan "") != (x_sqrt "")) 
			{++errs # cast to string as checking as double precision might show up very small differences
			 printf("  error %d: x=%g %s != %s\n",errs,x,(sin_atan ""),(x_sqrt ""))
			}
		}
	  printf("%d errors so far\n",errs)
	  
	  # now check power (^) using ^0.5 and comparing to sqrt
	  print "Checking x^0.5=sqrt(x):"
	  for(x=0;x<100;x+=0.125)
		{
		 x_pow_0p5=x^0.5
		 x_sqrt=sqrt(x)
		 printf(" x=%g: x^0.5=%.20g sqrt(x)=%.20g\n",x,x_pow_0p5,x_sqrt)
		 if((x_pow_0p5 "") != (x_sqrt "")) 
			{++errs # cast to string as checking as double precision might show up very small differences
			 printf("  error %d: x=%g %s != %s\n",errs,x,(x_pow_0p5 ""),(x_sqrt ""))
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
		 printf(" x=%g: exp(0.5*ln(x))=%.20g sqrt(x)=%.20g\n",x,e_l,s)
		 if(e_l "" != s "") 
			{++errs
			 printf("  error %d: x=%g %s != %s\n",errs,x,(e_l ""),( s ""))
			}
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
	 
	  # now do some timing (results just to stderr, whole scripts timing is captured in total time
	  # Blank loop() (baseline) - assume time per loop is constant when scaling to other values of xstart, xend, xinc
	  printf(" Timing for maths functions:\n") >"/dev/stderr"
	  ysum=0
	  xstart=0 # for loop
	  xend=25536.5
	  xstep=1.0/10091.0 #  1009, 10091 & 100907 are prime
	  startt=systime(1)	  
	  for(x=xstart;x<xend;x+=xstep) 
		{ysum+=(x)
		}
	  endt=systime(1)
	  baseline_time=endt-startt
	  printf("  Baseline - no function in loop : sum=%.20g time=%.2f secs\n",ysum,baseline_time)>"/dev/stderr"

	  
	  # sqrt()
	  ysum=0
	  # xstart,xend,xstep must be same as blank loop above
	  startt=systime(1)	  
	  for(x=xstart;x<xend;x+=xstep) 
		{ysum+=sqrt(x)
		}
	  endt=systime(1)
	  total_fun_time=(endt-startt)-baseline_time
	  time_per_call_ns= 1e9*total_fun_time/((xend-xstart)/xstep)
	  printf("  sqrt(x): sum=%.20g time for sqrt=%.2f secs (%.1f ns/call)\n",ysum, total_fun_time,time_per_call_ns)>"/dev/stderr"
	 # setup ready for scaling   baseline_time
	 baseline_time=baseline_time/((xend-xstart)/xstep) # time for 1 "blank" iteration
	 
	  # log()
	  ysum=0
	  xstart=1.0/10091.0 # for loop
	  xend=25536.5
	  xstep=1.0/10091.0 # 1009, 10091 & 100907 are prime	  
	  startt=systime(1)	  
	  for(x=xstart;x<xend;x+=xstep) 
		{ysum+=log(x)
		}
	  endt=systime(1)
	  total_fun_time=(endt-startt)-baseline_time*(xend-xstart)/xstep  # correct using scaled baseline_time
	  time_per_call_ns= 1e9*total_fun_time/((xend-xstart)/xstep)  
	  printf("  log(x): sum=%.20g time for log()=%.2f secs (%.1f ns/call)\n",ysum,total_fun_time,time_per_call_ns)>"/dev/stderr"	
	  
	  # exp()
	  ysum=0
	  xstart=-655 # for loop
	  xend=655
	  xstep=0.5/100907 # 1009, 10091 & 100907 are prime		  
	  startt=systime(1)	  
	  for(x=xstart;x<xend;x+=xstep) 
		{ysum+=exp(x)
		}
	  endt=systime(1)
	  total_fun_time=(endt-startt)-baseline_time*(xend-xstart)/xstep  # correct using scaled baseline_time
	  time_per_call_ns= 1e9*total_fun_time/((xend-xstart)/xstep)    
	  printf("  exp(x): sum=%.20g time for exp()=%.2f secs (%.1f ns/call)\n",ysum,total_fun_time,time_per_call_ns )>"/dev/stderr"	 
	  
	  # power(x,0.5)
	  ysum=0
	  xstart=0 # for loop
	  xend=655
	  xstep=0.5/100907 # 1009, 10091 & 100907 are prime		  
	  startt=systime(1)	  
	  for(x=xstart;x<xend;x+=xstep) 
		{ysum+=x^0.5
		}
	  endt=systime(1)	  
	  total_fun_time=(endt-startt)-baseline_time*(xend-xstart)/xstep  # correct using scaled baseline_time
	  time_per_call_ns= 1e9*total_fun_time/((xend-xstart)/xstep)      
	  printf("  power(x,0.5): sum=%.20g time for power()=%.2f secs (%.1f ns/call)\n",ysum,total_fun_time,time_per_call_ns )>"/dev/stderr"
	  
	  # Next calculate reference time for sin,cos which are used as args to atan2() [ don't do separate tests for sin() & cos() as we need this one for atan2() and doing separate ones for sin() & cos() as well would bias the total time towards sin/cos as they would be counted twice]
	  ysum=0
	  xstart=-1600 # for loop
	  xend=1600
	  xstep=1/100907 # 1009, 10091 & 100907 are prime		  
	  startt_ref=systime(1)	  
	  for(x=xstart;x<xend;x+=xstep) 
		{ysum+=sin(x)+cos(x)
		}
	  endt_ref=systime(1)	
	  total_fun_time=(endt_ref-ref-startt_ref)-baseline_time*(xend-xstart)/xstep  # correct using scaled baseline_time . Note time calculation is different as this is also used as baseline for atan2() which is next  
	  time_per_call_ns= 1e9*total_fun_time/((xend-xstart)/xstep)     # time for sin()x)+cos(x) 
	  printf("  sin() & cos() sum=%.20g time for sin+cos=%.2f secs (%.1f ns/call to either sin() or cos())\n",ysum,total_fun_time,time_per_call_ns/2.0 )>"/dev/stderr"	  # assumes time for sin and cos are equal (and time for "+" is very small) so divide time_per_call_ns by 2 
	  
	  # atan2(sin(x),cos(x)) proper, we subtract time of above loop so here we do really just time the atan2() function
	  ysum=0
	  # xstart,xend,xstep must be same as sin/cos above
	  startt=systime(1)	  
	  for(x=xstart;x<xend;x+=xstep) 
		{ysum+=atan2(sin(x),cos(x))
		}
	  endt=systime(1)	
	  total_fun_time=(endt-startt)-(endt_ref-startt_ref)  # correct using time for sin/cos .   
	  time_per_call_ns= 1e9*total_fun_time/((xend-xstart)/xstep)       	  
	  printf("  atan2() sum=%.20g time=%.2f secs (%.1f ns/call)\n",ysum,total_fun_time,time_per_call_ns )>"/dev/stderr"	  
	  
	  printf(" misc functions test finished %d errors found\n",errs)
	  printf(" misc functions test finished in %.2f secs , %d errors found\n", systime(1)-start_test,errs) >"/dev/stderr"
	  
	  if(errs>0) exit(1)
	  else exit(0)
	}