BEGIN{# simple test of unicode handling
	print "should print Dollar,Pound,Euro:$£€"
	print "should print Dollar,Pound,Euro:$£€" >"out2-$£€.txt"
	system("type out2-$£€.*")
	system("dir out2-$£€.*")
	"type out2-$£€.txt" | getline variable
	print "output of pipe was:",variable
	print "Command line arguments are:"
	for(j=0;j<ARGC;++j)
		printf(" ARGV[%u]=%s\n",j,ARGV[j])
	print "environment variables are:"
	for(i in ENVIRON)
		printf(" ENVIRON[%s]=%s\n",i,ENVIRON[i])
	}