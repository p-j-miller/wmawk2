BEGIN {
		file1="temp1.out"
		print "3">file1
		print "2">file1
		print "1">file1
		close(file1)
        input = "type " file1
	    output = "sort"

	    while((input | getline > 0)) {
	        print | output
	    }
	    print close(input), close(output)
		system("del " file1)
		
		file = "foo" rand()
		printf "%x %x %d\n", -1, 2^32, -2^32 > file
		print close(file)
		command = sprintf("type %s", file)
		print system(command)
		system("del " file)
}




