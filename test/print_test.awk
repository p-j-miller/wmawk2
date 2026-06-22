BEGIN{ # tests changes in wmawk2 2v1 using fpfmt etc
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
	 init()
	 start_t=systime(1)
	 # 1e6 gives ~ 112 secs with wamwk2-2v0, this is a reasonable match in duration to the memory stress test later (99 secs)
     for(i=0;i<=max_test_nos+1e6;++i)	
		{if(i%100000 ==0) printf("*") >"/dev/stderr" # give user something to watch print * every 1e5 so should print 50 *'s
		 t=i<=max_test_nos?test_nos[i]:(prand()*2e10-1e10) # either value from "table" or random number +/-1e10
		 # cast to string
		 c=t+""
		 # sprintf
		 sd=sprintf("%d",t )
		 su=sprintf("%u",t)
		 
		 se=sprintf("%e",t)
		 se0=sprintf("%.0e",t)
		 se1=sprintf("%.1e",t)
		 se2=sprintf("%.2e",t)
		 se3=sprintf("%.3e",t)
		 se4=sprintf("%.4e",t)
		 se5=sprintf("%.5e",t)
		 se6=sprintf("%.6e",t)
		 se7=sprintf("%.7e",t)
		 se8=sprintf("%.8e",t)
		 se9=sprintf("%.9e",t)
		 se10=sprintf("%.10e",t)
		 se11=sprintf("%.11e",t)
		 se12=sprintf("%.12e",t)
		 se13=sprintf("%.13e",t)
		 se14=sprintf("%.14e",t)
		 se15=sprintf("%.15e",t)
		 se16=sprintf("%.16e",t)

		 sf=sprintf("%f",t)
		 sf0=sprintf("%.0f",t)
		 sf1=sprintf("%.1f",t)
		 sf2=sprintf("%.2f",t)
		 sf3=sprintf("%.3f",t)
		 sf4=sprintf("%.4f",t)
		 sf5=sprintf("%.5f",t)
		 sf6=sprintf("%.6f",t)
		 sf7=sprintf("%.7f",t)
		 sf8=sprintf("%.8f",t)
		 sf9=sprintf("%.9f",t)
		 sf10=sprintf("%.10f",t)
		 sf11=sprintf("%.11f",t)
		 sf12=sprintf("%.12f",t)
		 sf13=sprintf("%.13f",t)
		 sf14=sprintf("%.14f",t)
		 sf15=sprintf("%.15f",t)
		 sf16=sprintf("%.16f",t)
		 sf17=sprintf("%.17f",t)
		 
		 sg=sprintf("%g",t)
		 sg0=sprintf("%.0g",t)
		 sg1=sprintf("%.1g",t)
		 sg2=sprintf("%.2g",t)
		 sg3=sprintf("%.3g",t)
		 sg4=sprintf("%.4g",t)
		 sg5=sprintf("%.5g",t)
		 sg6=sprintf("%.6g",t)
		 sg7=sprintf("%.7g",t)
		 sg8=sprintf("%.8g",t)
		 sg9=sprintf("%.9g",t)

		 sg10=sprintf("%.10g",t)
		 sg11=sprintf("%.11g",t)
		 sg12=sprintf("%.12g",t)
		 sg13=sprintf("%.13g",t)
		 sg14=sprintf("%.14g",t)
		 sg15=sprintf("%.15g",t)
		 sg16=sprintf("%.16g",t)
		 sg17=sprintf("%.17g",t)
		 
		 
		 s1e=sprintf("%1e",t) # not accelerated
		 s1g=sprintf("%1g",t) # not accelerated
		 sE=sprintf("%E",t) # not accelerated
		 sE1=sprintf("%.1E",t) # not accelerated
		 sE10=sprintf("%.10E",t) # not accelerated
		 sE16=sprintf("%.16E",t) # not accelerated
		 sG=sprintf("%G",t) # not accelerated		
		 sG1=sprintf("%.1G",t) # not accelerated
		 sG10=sprintf("%.10G",t) # not accelerated
		 sG17=sprintf("%.17G",t) # not accelerated
		 
		 # do some basic round the loop tests
		 # DBL_DECIMAL_DIG is 17 so %.16e or %.17g should be round the loop exact
		 # no %f is guaranteed to be round the loop exact in general, here we check values >1
		 if(se16+0!=t)
			{errs++
			 printf("Error: se16=%s != t(%.16e - %.17g)\n",se16,t,t)>>"/dev/stderr"
			}
		 if(sg17+0!=t)
			{errs++
			 printf("Error: sg17=%s != t(%.16e - %.17g)\n",sg17,t,t)>>"/dev/stderr"
			}
		 if(t>=1 && sf17+0!=t)
			{errs++
			 printf("Error: sf17=%s != t(%.16e - %.17g, %.17f)\n",sf17,t,t,t)>>"/dev/stderr"
			}
		 # test print statement
		 print "print",i,t,c,sd,su,"SE",se,se0,se1,se2,se3,se4,se5,se6,se7,se8,se9, se10,se11,se12,se13,se14,se15,se16,"SF",sf,sf0,sf1,sf2,sf3,sf4,sf5,sf6,sf7,sf8,sf9, sf10,sf11,sf12,sf13,sf14,sf15,sf16,sf17,"SG",sg,sg0,sg1,sg2,sg3,sg4,sg5,sg6,sg7,sg8,sg9, sg10,sg11,sg12,sg13,sg14,sg15,sg16,sg17,"not acc",s1e,s1g,sE,sE1,sE10,sE16,sG,sG1,sG10,sG17
		 
		 # test printf statement
		 printf("printf %d : \"%s\" ",i,c )
		 printf("%d ",t )
		 printf("%u ",t)
		 printf("e: %e ",t)
		 printf("%.0e ",t)
		 printf("%.1e ",t)
		 printf("%.2e ",t)
		 printf("%.3e ",t)
		 printf("%.4e ",t)
		 printf("%.5e ",t)
		 printf("%.6e ",t)
		 printf("%.7e ",t)
		 printf("%.8e ",t)
		 printf("%.9e ",t)

		 printf("%.10e ",t)
		 printf("%.11e ",t)
		 printf("%.12e ",t)
		 printf("%.13e ",t)
		 printf("%.14e ",t)
		 printf("%.15e ",t)
		 printf("%.16e ",t)
		 
		 printf("f: %f ",t)
		 printf("%.0f ",t)
		 printf("%.1f ",t)
		 printf("%.2f ",t)
		 printf("%.3f ",t)
		 printf("%.4f ",t)
		 printf("%.5f ",t)
		 printf("%.6f ",t)
		 printf("%.7f ",t)
		 printf("%.8f ",t)
		 printf("%.9f ",t)
		 printf("%.10f ",t)
		 printf("%.11f ",t)
		 printf("%.12f ",t)
		 printf("%.13f ",t)
		 printf("%.14f ",t)
		 printf("%.15f ",t)
		 printf("%.16f ",t)
		 printf("%.17f ",t)
		 
		 printf("g: %g ",t)
		 printf("%.0g ",t)
		 printf("%.1g ",t)
		 printf("%.2g ",t)
		 printf("%.3g ",t)
		 printf("%.4g ",t)
		 printf("%.5g ",t)
		 printf("%.6g ",t)
		 printf("%.7g ",t)
		 printf("%.8g ",t)
		 printf("%.9g ",t)

		 printf("%.10g ",t)
		 printf("%.11g ",t)
		 printf("%.12g ",t)
		 printf("%.13g ",t)
		 printf("%.14g ",t)
		 printf("%.15g ",t)
		 printf("%.16g ",t)
		 printf("%.17g ",t)
	 
		 
		 printf("not acc: %1e ",t) # not accelerated
		 printf("%1g ",t) # not accelerated
		 printf("%E ",t) # not accelerated
		 printf("%.1E ",t) # not accelerated
		 printf("%.10E ",t) # not accelerated
		 printf("%.16E ",t) # not accelerated
		 printf("%G ",t) # not accelerated		
		 printf("%.1G ",t) # not accelerated
		 printf("%.10G ",t) # not accelerated	
		 printf("%.17G ",t) # not accelerated
		 printf("\n")	
		}
	 end_t=systime(1)		
	 print "test finished",errs+0,"errors found" # cannot print time to file as we using file compare to check results and different times would cause this to fail
	 print >"/dev/stderr"
	 print "test finished in",end_t-start_t,"seconds",errs+0,"errors found" >"/dev/stderr"		
	 if(errs+0==0) exit(0) 
	 else exit(1) # pass fail readable by calling batch file via %errorlevel%
	}


function prand() # Based on the portable random number generator from Numerical recipes 1st edition.  
		# Its advantage is that it should give identical results with all awk interpreters
		# Constants from table 3 in "TABLES OF LINEAR CONGRUENTIAL GENERATORS OF DIFFERENT SIZES AND GOOD LATTICE STRUCTURE" by PIERRE L’ECUYER, Jan 1999
		# Do not use this for any cryptographic applications, but its period of 34,359,738,337 should be adequate for many applications.
		# It was developed to generate test cases for various algorithms (like sort, median).
		# it generates a uniformly distributed random number between 0 and 1. 0 and 1 will never be generated.
		# Note Cliff random number generator is sometimes suggested for this type of application - but the author found this did not generate uniformly distributed random number between 0 and 1 
		# (it does does generate numbers between 0 and 1 but they are not uniformly distributed so for example do not give an average of 0.5 when a large number of values are generated).
		# Taking the average of a large number of values returned by prand() will give a number very close to 0.5
{if(rand_value<=0) rand_value=2147483647 # 2^31-1 is used as a seed - it could be any value >0. rand_value is a global variable, that's automatically created when required.
 rand_value=int((rand_value*185852) %34359738337 ) # rand_value*(m-1)<2^53 so will fit exactly in an ieee double 
 return rand_value/34359738337  # returns a number >0..<1   
}	

function init() # initialise test_nos array
{
	test_nos[0]= -1.79769313486231570814527423731704357e+308 # most negative floating point number
	test_nos[1]= -9007199254740991.0 # largest negative number mawk2 considers as an integer
	test_nos[2]=-10000000 # -10,000,000 should not be printed by fast integer routines
	test_nos[3]= -9999999 #  -9,999,999 most negative number that should be printed by fast integer routines
	test_nos[4]= -1
	test_nos[5]=0 # for most of the printing routines zero is a special case
	test_nos[6]=1
	test_nos[7]=99999999 #  99,999,999 most positive number that should be printed by fast integer routines
	test_nos[8]=100000000 # 100,000,000  should not be printed by fast integer routines
	test_nos[9]= 9007199254740991.0 # largest positive number mawk2 considers as an integer
	test_nos[10]= 1.79769313486231570814527423731704357e+308 # most positive floating point number
	# double specific values
	test_nos[11]=4.94065645841246544176568792868221372e-324 # denorm min
	test_nos[12]=2.22507385850720138309023271733240406e-308 # double min
	test_nos[12]=1.0+2.22044604925031308084726333618164062e-16 # dbl_epsilon
	test_nos[13]=4.94065645841246544176568792868221372e-324 # denorm min
	test_nos[14]=2.22507385850720138309023271733240406e-308 # double min
	test_nos[15]=1.0+2.22044604925031308084726333618164062e-16 # dbl_epsilon
	# special values for %f
	test_nos[16]=0.6
	test_nos[17]=0.997
	test_nos[18]=517.5
	test_nos[19]=67.6875
    max_test_nos=19	
}