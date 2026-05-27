@echo off
rem basic wmawk2 test batch file - tests mawk2 version given as param to batch file
rem Written by Peter Miller 21/5/2026
rem warning : print_test.out is a large (1.4GB) file - so one of the temp.out files will be the same size
rem errorlevel==0 is good. <>0 is bad, so use "if not %errorlevel% == 0" (don't use "if %errorlevel% == 1" as many programs return different non-zero values depending on the actual error)
if "%1"=="" goto noparam:
rem print version we are testing
echo Testing version:
%1 -W version
echo.
rem save start time, note that we need %% in the line below as % by itself is considered a batch file variable
%1 "BEGIN{printf \"%%.18g\n\", systime(1)}" >start_time.out
if not %errorlevel% == 0 goto error0
echo testing program on command line
rem note that we need %%d in the line below as %d by itself is considered a batch file variable
%1  "BEGIN{for(i=0;i<10;++i) printf(\"i=%%d\n\",i)}" >temp.out
if not %errorlevel% == 0 goto error0
comp temp.out commline.out /m >NUL
if not %errorlevel% == 0 goto error0
echo  program on command line OK
echo.

echo testing input and field splitting
%1 -f wc.awk mawktest.dat >temp.out
if not %errorlevel% == 0 goto error1
comp temp.out wc-awk.out /m >NUL
if not %errorlevel% == 0 goto error1
rem now try with stdin redirected
%1 -f wc.awk <mawktest.dat >temp.out
if not %errorlevel% == 0 goto error1
comp temp.out wc-awk.out /m >NUL
if not %errorlevel% == 0 goto error1
rem now try with input piped in
type mawktest.dat | %1 -f wc.awk  >temp.out
if not %errorlevel% == 0 goto error1
comp temp.out wc-awk.out /m >NUL
if not %errorlevel% == 0 goto error1
echo  input and field splitting OK
echo.

echo testing regular expression matching
%1 -f reg0.awk mawktest.dat > temp.out
if not %errorlevel% == 0 goto error2
%1 -f reg1.awk mawktest.dat >> temp.out
if not %errorlevel% == 0 goto error2
%1 -f reg2.awk mawktest.dat >> temp.out
if not %errorlevel% == 0 goto error2
%1 -f reg3.awk mawktest.dat >> temp.out
if not %errorlevel% == 0 goto error2
comp temp.out reg-awk.out /m >NUL
if not %errorlevel% == 0 goto error2
echo  regular expression matching OK
echo.

echo testing arrays and flow of control
%1 -f wfrq0.awk mawktest.dat > temp.out
if not %errorlevel% == 0 goto error3
comp temp.out wfrq-awk.out /m >NUL
if not %errorlevel% == 0 goto error3
echo  array test OK
echo.

echo testing input/output with pipes
%1 -f pipetest.awk  > temp.out
if not %errorlevel% == 0 goto error4
comp temp.out pipetest.out /m >NUL
if not %errorlevel% == 0 goto error4
echo  input/output with pipes test OK
echo.

echo testing function calls and general stress test
%1 -f ..\examples\decl.awk mawktest.dat >temp.out
if not %errorlevel% == 0 goto error5
comp temp.out decl-awk.out /m >NUL
if not %errorlevel% == 0 goto error5
echo  general stress test passed
echo.

echo testing floating point exceptions
%1 -f fpetest1.awk >temp.out
if not %errorlevel% == 0 goto error6
comp temp.out fpetest1.out /m >NUL
if not %errorlevel% == 0 goto error6
%1 -f fpetest2.awk >temp.out
if not %errorlevel% == 0 goto error6
comp temp.out fpetest2.out /m >NUL
if not %errorlevel% == 0 goto error6
rem fpetest3 should create a nan - the format for this changed with 2v1 from "nan" to "-nan(ind)"
%1 -f fpetest3.awk >temp.out
if not %errorlevel% == 0 goto error6
comp temp.out fpetest3.out /m >NUL
if %errorlevel%==0 goto ok6
rem if files not identical, check line includes "nan"
find "nan" temp.out >NUL
if not %errorlevel% == 0 goto error6
:ok6
echo  floating point exceptions test passed
echo.

echo testing stdin,stdout and stderr
echo 1 2 3 | %1 -f stdin.awk  >temp1.out 2>temp2.out
if not %errorlevel% == 0 goto error7
comp temp1.out stdin1.out /m >NUL
if not %errorlevel% == 0 goto error7
comp temp2.out stdin2.out /m >NUL
if not %errorlevel% == 0 goto error7
del temp1.out
del temp2.out
echo  stdin,stdout and stderr test passed
echo.

echo test/time print,printf,sprintf and casting numbers to strings in awk program
echo will print *'s relatively slowly... to match line below:
echo ***********
%1 -f print_test.awk >temp.out
if not %errorlevel% == 0 goto error8
comp temp.out print_test.out /m >NUL
if not %errorlevel% == 0 goto error8
echo  test/time print,printf,sprintf and casting numbers to strings test passed
echo.

echo testing use of unicode characters
rem program uses the environ variable below, and also uses/checks unicode.awk in the awk library
set wmawk2-env="Unicode$£€test"
%1 -f D:\AWK-library\unicode.awk -f Unicode$£€test.awk -- Dollar Pound Euro $ £ € >temp.out
if not %errorlevel% == 0 goto error9
comp temp.out Unicode$£€test.out /m >NUL
if not %errorlevel% == 0 goto error9
echo  use of unicode characters test passed
echo.

echo testing reading a large file
rem mainly a speed test, but does check reading numbers
%1 -f min-max-avg.awk print_test.out >temp.out
if not %errorlevel% == 0 goto error10
comp temp.out min-max-avg.out /m >NUL
if not %errorlevel% == 0 goto error10
echo  reading a large file test passed
echo.

echo stress test of memory allocation 
echo will print 6 *'s (slowly!) to match line below:
echo ******
rem mainly a memory allocation test, but does check a lot of functionality including recursive function calls in qsort and loading multiple programs 
%1 -f D:\AWK-library\median.awk -f D:\AWK-library\qsort.awk -f D:\AWK-library\prand.awk -f median_test.awk >temp.out
if not %errorlevel% == 0 goto error11
comp temp.out median_test.out /m >NUL
if not %errorlevel% == 0 goto error11
echo  stress test of memory allocation passed
echo.

echo test string handling and maths 
%1 -f D:\AWK-library\strftime.awk  -f strftime_test.awk >temp.out
if not %errorlevel% == 0 goto error12
comp temp.out strftime_test.out /m >NUL
if not %errorlevel% == 0 goto error12
echo  test of string handling and maths passed
echo.

echo testing misc functions not already tested 
%1  -f miscfun_test.awk >temp.out
if not %errorlevel% == 0 goto error13
comp temp.out miscfun_test.out /m >NUL
if not %errorlevel% == 0 goto error13
echo  test of misc functions not already tested passed
echo.

echo All tests passed
%1 "{print \"Total test time was\",systime(1)-$1,\"secs\"}" start_time.out
del temp.out
rem del start_time.out
exit /b

:noparam
echo Error - missing paramater (wmawk to run)
exit /b
:error0
echo program on command line failed
exit /b
:error1
echo input and field splitting failed
exit /b
:error2
echo regular expression matching failed
exit /b
:error3
echo array test failed
exit /b
:error4
echo input/output with pipes test failed
exit /b
:error5
echo general stress test failed
exit /b
:error6
echo floating point exceptions test failed
exit /b
:error7
echo stdin,stdout and stderr test failed
exit /b
:error8
echo test/time print,printf,sprintf and casting numbers to strings test failed
exit /b
:error9
echo use of unicode characters test failed
exit /b
:error10
echo reading a large file test failed
exit /b
:error11
echo stress test of memory allocation failed
exit /b
:error12
echo test of string handling and maths failed
exit /b
:error13
echo test of misc functions not already tested failed
exit /b
