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
BEGIN{# simple test of unicode handling
	print "should print Dollar,Pound,Euro:$£€"
	print "should print Dollar,Pound,Euro:$£€" >"out2-$£€.txt"
	system("type out2-$£€.txt")
	system("dir /B out2-$£€.*")
	"type out2-$£€.txt" | getline variable
	print "output of pipe was:",variable
	print "Command line arguments are:"
	for(j=1;j<ARGC;++j)
		printf(" ARGV[%u]=%s\n",j,ARGV[j])
	print "environment variable is:"
	for(i in ENVIRON)
		if(i=="wmawk2-env") printf(" ENVIRON[%s]=%s\n",i,ENVIRON[i])
	}
	
BEGIN { # test program from awk library -  execute as wmawk2 -f unicode.awk -f unicode$£€test.awk
	  errs=0
	  for(n=0;n<256;n++) t = t chr(n) # create a byte array in string t for testing
	  
	  for(n=0;n<256;++n)
		if(ord(substr(t,n+1,1))!=n) {errs++;printf("Error at n=%u (0x%x) ord() returned %u (0x%x)\n",n,n,ord(substr(t,n+1,1)),ord(substr(t,n+1,1))) } # check we can convert a string of 8 bit bytes back to the correspnding integers
	  # check a few random entries - just in case!
	  var=""
	  ovar=ord(var)
	  printf("ord(\"%s\"=%d (0x%x) [ this is a zero length string ]\n",var,ovar,ovar)
	  if(ovar!=-1 ) { errs++;printf("   Above is incorrect!\n")}	  
	  var="a"
	  ovar=ord(var)
	  printf("ord(\"%s\"=%d (0x%x) : %%c gives %c\n",var,ovar,ovar,ovar)
	  if(ovar!=97 ||sprintf("%c",ovar)!=var) { errs++;printf("   Above is incorrect!\n")}
	  var="Z"
	  ovar=ord(var)
	  printf("ord(\"%s\"=%d (0x%x) : %%c gives %c\n",var,ovar,ovar,ovar)
	  if(ovar!=90 ||sprintf("%c",ovar)!=var) { errs++;printf("   Above is incorrect!\n")}
	  var="0"
	  ovar=ord(var)
	  printf("ord(\"%s\"=%d (0x%x) : %%c gives %c\n",var,ovar,ovar,ovar)
	  if(ovar!=48 ||sprintf("%c",ovar)!=var) { errs++;printf("   Above is incorrect!\n")}
	  var="9"
	  ovar=ord(var)
	  printf("ord(\"%s\"=%d (0x%x) : %%c gives %c\n",var,ovar,ovar,ovar)
	  if(ovar!=57 ||sprintf("%c",ovar)!=var) { errs++;printf("   Above is incorrect!\n")}

	  # tests for u8length(ustr)
	  delete _ord_ # make sure initialisation works correctly for u8length()
	  s="1234"
	  printf("u8length(\"%s\") returns %d, length(\"%s\") returns %d\n",s,u8length(s),s,length(s))
	  delete _ord_ # make sure initialisation works correctly for lenu8chr()
	  i=1; cnt=0
	  while(i<=length(s))
		{l=lenu8chr(substr(s,i)) # length of next character

		 c=substr(s,i,l)
		 printf(" Character %d = \"%s\" at index %d length %d\n",++cnt,c,i,l)
		 if(c!=u8substr(s,cnt,1)) {errs++;printf(" Error: lenu8char()[=\"%s\"] and u8substr() [=\"%s\"] disagree on the %d character\n",c,u8substr(s,cnt,1),cnt) }
		 if(l!=1) {errs++; printf(" Error: length is wrong got %d should be 1\n",l)}
		 i+=l
		}
	  if(cnt!=u8length(s)) {errs++;printf("Error: number of characters from u8length=%d while number of characters from sum lenu8chr() is %d\n",u8length(s),cnt)}
	  
	  s="$£€123$£€"
	  printf("u8length(\"%s\") returns %d, length(\"%s\") returns %d\n",s,u8length(s),s,length(s))
	  if(u8length(s)!=9) 
		{errs++;printf("Error: expected utf8 length of \"%s\" to be 9 (got %d)\n",s,u8length(s))  }
	  if(length(s)!=15) 
		{errs++;printf("Error: expected (byte) length() of \"%s\" to be 15 (got %d)\n", s,length(s)) }
	  if(length(s)==9) 
		{errs++;printf("  it appears that length(\"%s\") has returned the number of utf8 characters not the number of bytes\n")  }
	  i=1; cnt=0
	  while(i<=length(s))
		{l=lenu8chr(substr(s,i)) # length of next character
		 c=substr(s,i,l)
		 printf(" Character %d = \"%s\" at index %d length %d\n",++cnt,c,i,l)
		 if(c!=u8substr(s,cnt,1)) {errs++;printf("Error: lenu8char()[=\"%s\"] and u8substr() [=\"%s\"] disagree on the %d character\n",c,u8substr(s,cnt,1),cnt)}
		 if(cnt==1 && l!=1) {errs++; printf(" Error: length is wrong got %d should be 1\n",l)}
		 if(cnt==1 && c!="$") {errs++; printf(" Error: character is wrong got %s should be $\n",c)}
		 if(cnt==2 && l!=2) {errs++; printf(" Error: length is wrong got %d should be 2\n",l)}
		 if(cnt==2 && c!="£") {errs++; printf(" Error: character is wrong got %s should be £\n",c)}
		 if(cnt==3 && l!=3) {errs++; printf(" Error: length is wrong got %d should be 3\n",l)}
		 if(cnt==3 && c!="€") {errs++; printf(" Error: character is wrong got %s should be €\n",c)}
		 if(cnt==4 && l!=1) {errs++; printf(" Error: length is wrong got %d should be 1\n",l)}
		 if(cnt==4 && c!="1") {errs++; printf(" Error: character is wrong got %s should be 1\n",c)}
		 if(cnt==5 && l!=1) {errs++; printf(" Error: length is wrong got %d should be 1\n",l)}
		 if(cnt==5 && c!="2") {errs++; printf(" Error: character is wrong got %s should be 2\n",c)}
		 if(cnt==6 && l!=1) {errs++; printf(" Error: length is wrong got %d should be 1\n",l)}
		 if(cnt==6 && c!="3") {errs++; printf(" Error: character is wrong got %s should be 3\n",c)}
		 if(cnt==7 && l!=1) {errs++; printf(" Error: length is wrong got %d should be 1\n",l)}
		 if(cnt==7 && c!="$") {errs++; printf(" Error: character is wrong got %s should be $\n",c)}
		 if(cnt==8 && l!=2) {errs++; printf(" Error: length is wrong got %d should be 2\n",l)}
		 if(cnt==8 && c!="£") {errs++; printf(" Error: character is wrong got %s should be £\n",c)}
		 if(cnt==9 && l!=3) {errs++; printf(" Error: length is wrong got %d should be 3\n",l)}
		 if(cnt==9 && c!="€") {errs++; printf(" Error: character is wrong got %s should be €\n",c)}
		 i+=l
		}
	  if(cnt!=u8length(s)) {errs++;printf("Error: number of characters from u8length=%d while number of characters from sum lenu8chr() is %d\n",u8length(s),cnt)}
	  
	  s="Ḽơᶉëᶆ"
	  printf("u8length(\"%s\") returns %d, length(\"%s\") returns %d\n",s,u8length(s),s,length(s))
	  if(u8length(s)!=5) 
		{errs++;printf("Error: expected utf8 length of \"%s\" to be 5 (got %d)\n",s,u8length(s))  }
	  if(length(s)!=13) 
		{errs++;printf("Error: expected (byte) length() of \"%s\" to be 13 (got %d)\n", s,length(s)) }
	  if(length(s)==5) 
		{errs++;printf("  it appears that length(\"%s\") has returned the number of utf8 characters not the number of bytes\n")  }	  
	  i=1; cnt=0
	  while(i<=length(s))
		{l=lenu8chr(substr(s,i)) # length of next character
		 c=substr(s,i,l)
		 printf(" Character %d = \"%s\" at index %d length %d\n",++cnt,c,i,l)
		 if(c!=u8substr(s,cnt,1)) {errs++;printf("Error: lenu8char()[=\"%s\"] and u8substr() [=\"%s\"] disagree on the %d character\n",c,u8substr(s,cnt,1),cnt)}
		 i+=l
		}
	  if(cnt!=u8length(s)) {errs++;printf("Error: number of characters from u8length=%d while number of characters from sum lenu8chr() is %d\n",u8length(s),cnt)  }
	  
	  s="" # zero length string
	  printf("u8length(\"%s\") returns %d, length(\"%s\") returns %d, lenu8chr(\"%s\") returns %d \n",s,u8length(s),s,length(s),s,lenu8chr(s))
	  if(u8length(s)!=0) errs++
	  if(length(s)!=0) errs++
	  if(lenu8chr(s)!=0) errs++
	  i=1; cnt=0
	  while(i<=length(s))
		{l=lenu8chr(substr(s,i)) # length of next character
		 c=substr(s,i,l)
		 printf(" Character %d = \"%s\" at index %d length %d\n",++cnt,c,i,l)
		 if(c!=u8substr(s,cnt,1)) {errs++;printf("Error: lenu8char()[=\"%s\"] and u8substr() [=\"%s\"] disagree on the %d character\n",c,u8substr(s,cnt,1),cnt)}
		 i+=l
		}
	  if(cnt!=u8length(s)) {errs++;printf("Error: number of characters from u8length=%d while number of characters from sum lenu8chr() is %d\n",u8length(s),cnt)}
	  
	  #check a numeric argument
	  printf("length(15*35)=%d u8length(15*35)=%d : both are expected to be 3 as 15*35=525\n",length(15*35),u8length(15*35))
	  if(length(15*35)!=3) errs++
	  if(u8length(15*35)!=3) errs++
	  # now test function u8substr(ustr, ustart , ulength      , i,j,cnt,l)
	  s="Ḽơᶉëᶆ"
	  for(us=0;us<=u8length(s)+1;++us)
		{# first get the whole suffix by omitting ulength
		 printf("u8substr(\"%s\",start=%d) returns \"%s\"\n",s,us,u8substr(s,us))
		 for(ul=0;ul<=u8length(s)-us+2;++ul) # +1 is all that's strictly needed , +2 checks what happens if ulength is too big
			{printf("u8substr(\"%s\",start=%d,length=%d) returns \"%s\"\n",s,us,ul,u8substr(s,us,ul))
			 # check a few cases
			 if(length==2)
				{if(start==0 && u8substr(s,us,ul)!="Ḽơ") {errs++;printf("  Error: should be Ḽơ\n")}
				 if(start==1 && u8substr(s,us,ul)!="Ḽơ") {errs++;printf("  Error: should be Ḽơ\n")}
				 if(start==2 && u8substr(s,us,ul)!="ơᶉ") {errs++;printf("  Error: should be ơᶉ\n")}
				 if(start==3 && u8substr(s,us,ul)!="ᶉë") {errs++;printf("  Error: should be ᶉë\n")}
				 if(start==4 && u8substr(s,us,ul)!="ëᶆ") {errs++;printf("  Error: should be ëᶆ\n")}
				 if(start==5 && u8substr(s,us,ul)!="ᶆ") {errs++;printf("  Error: should be ᶆ\n")}
				} 
			}
		}
	 if(errs==0) 
		{printf("  Unicode Test finished - no errors found\n") >"/dev/stderr"
		 printf("Unicode Test finished - no errors found\n")
		 exit(0)
		}
	 else
		{printf("  Unicode Test finished - %d errors found in tests\n",errs) >"/dev/stderr"
		 printf("Unicode Test finished -%d errors found in tests\n",errs)
		 exit(1)
		}
	}	