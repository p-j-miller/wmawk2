# median_test.awk
# test code for median.awk
# This by Peter Miller 25-8-2024 is placed in the public domain

# run with wmawk2 -f median.awk -f qsort.awk -f prand.awk -f median_test.awk
# This version for use in wmawk2test.bat - only prints time to stderr and returns exit status of 0/1 depending on errors

function test1(arr,maxi,    pos_median,med_torben,i,mint,maxt,sizet) # note errs is a global variable
{# test arr with maxi elements and a subset in the middle -increments errs whenever an error found
 printf("*") >"/dev/stderr" # give user something to see !
 if(maxi>201)
	{# also test a segment in the middle
     mint=100
	 maxt=201
	 sizet=1+maxt-mint
	 pos_median=mint+int((sizet+1)/2)
	 med_torben=_median(arr,mint,maxt)
	 _qsort(arr,mint,maxt)
	 if(med_torben!=arr[pos_median]) {errs++; printf("   Error: _median(arr,%d,%d) returned %g but arr[%d]=%g\n",mint,maxt,med_torben,pos_median,arr[pos_median]) }
	 else printf("  _median(arr,%d,%d) returned %g and arr[%d]=%g\n",mint,maxt,med_torben,pos_median,arr[pos_median])	 
	}
 # now test using whole array
 pos_median=int((maxi+1)/2)
 med_torben=median(arr,maxi)	
 qsort(arr,maxi) # sort array so we can get median as middle element
 if(med_torben!=arr[pos_median]) {errs++; printf("   Error: median() returned %g but arr[%d]=%g\n",med_torben,pos_median,arr[pos_median]) }
 else printf("  median() returned %g and arr[%d]=%g\n",med_torben,pos_median,arr[pos_median])
 
 # check number of elements unchanged by median/sort
 if(length(arr)!=maxi) {errs++;printf("Number of elements in arr changed - should be %d found %d\n",length(arr),maxi)}
 # now check result is sorted - nor normally needed, so commented out, but useful if the test above fails to make sure sort worked correctly
 #for(i=2;i<=maxi;++i)
 #	if(arr[i-1]>arr[i]) {errs++;printf("Array not properly sorted arr[%d]=%g arr[%d]=%g\n",i-1,arr[i-1],i,arr[i])}
 return med_torben # allow caller to check result
}

# Actual test code
BEGIN { 
 errs=0
 start_time=systime(1) # systime(1) gives UTC time to 1us resolution
 maxi=10000001 # number of array elements to sort, should be odd.  maxi=10000001 takes 74 secs on my PC
 pos_median=int((maxi+1)/2)
 
 printf("Testing median() for %d elements so median at array index %d\n",maxi,pos_median)
 print " testing random numbers 0-1 so expect median to be ~0.5"
 for(i=1;i<=maxi;++i)
	arr[i]=prand() # fill array with random positive numbers 0..1
 r=test1(arr,maxi)
 if(r<0.49 || r>0.51) {errs++; printf("  Error - expected median to be 0.49..0.51\n")}	

 print " testing random numbers +/-1 so expect median to be ~0"	
 for(i=1;i<=maxi;++i)
	arr[i]=1-2*prand() # fill array with random positive numbers +/-1
 r=test1(arr,maxi)
 if(r<-0.01 || r>0.01) {errs++; printf("  Error - expected median to be -0.01..0.01\n")}	

 # testing integer numbers 0..maxi-1 (so already sorted)
 for(i=1;i<=maxi;++i)
	arr[i]=i-1 # fill array with integers 0..maxi-1
 med=arr[pos_median]  # already sorted so know result
 print " testing integer numbers 0..maxi-1 so expect median to be",med
 r=test1(arr,maxi)
 if(r!=med) {errs++; printf("  Error - expected median to be %g\n",med)}
 
 print " testing integer numbers maxi-1 to 0 so expect median to be",med
 for(i=1;i<=maxi;++i)
	arr[i]=maxi-i # fill array with integers maxi-1..0
 r=test1(arr,maxi)
 if(r!=med) {errs++; printf("  Error - expected median to be %g\n",med)}
 
 print " testing integer numbers all 5, so expect median to be 5"
 for(i=1;i<=maxi;++i)
	arr[i]=5 # fill array with integers 5
 r=test1(arr,maxi)
 if(r!=5) {errs++; printf("  Error - expected median to be 5\n")}	
 
 print " testing integer numbers mainly 5, with a few other values - so expect median to be 5"
 for(i=1;i<=maxi;++i)
	arr[i]=5 # fill array with integers 5
 for(i=1;i<10;++i)
	arr[int(prand()*maxi)]=i # add in a few other values at random locations
 r=test1(arr,maxi)
 if(r!=5) {errs++; printf("  Error - expected median to be 5\n")}	
 # expected final values from random number generator - add values for different maxi as required
 if(maxi==10000001)
   expected_rand_value=20984299426 # 0x4e2c2ffa2
 else if(maxi==20000001)
   expected_rand_value=3075012908
 else if(maxi==1000001)
   expected_rand_value=2091721999 
 else if(maxi==2000001)
   expected_rand_value=32355024622     
 else
	{printf("Random number generators final value was %d - correct value is unknown for maxi=%d\n",rand_value,maxi)
	 expected_rand_value=0
	}
 if(expected_rand_value!=0 && rand_value!=expected_rand_value) {errs++; printf("Warning incorrect value from random number generator - got %d expected %d\n Have you changed the test program?\n",rand_value,expected_rand_value)}
 
 if(errs==0) 
	{printf("\n") >"/dev/stderr"
	 print "median() test finished OK"
	 print "median() test finished OK, total execution time was",systime(1)-start_time,"seconds" >"/dev/stderr"
	 exit(0)
	}
 else
	{printf("\n") >"/dev/stderr"
	 print "median() test finished with",errs,"error(s)"
	 print "median() test finished with",errs,"error(s), total execution time was",systime(1)-start_time,"seconds" >"/dev/stderr"
	 exit(1)
	}	
}
