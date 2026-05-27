BEGIN{ # test special filenames
	  getline line < "-"
	  print "print with no redirection gives",line
	  print "print redirected to stdout gives",line >"/dev/stdout"
	  print "print redirected to stderr gives",line >"/dev/stderr"
	  }