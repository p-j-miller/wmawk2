# create a file with a NULL character to check that wmawk2 can read it OK
BEGIN 	{printf( "BEGIN {print \"Hello %c World\" }\n",0) > "test1.awk" 
		}

       