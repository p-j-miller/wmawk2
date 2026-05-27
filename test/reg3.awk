# same as reg2.awk, but using character classes to replace /[A-Za-z_][A-Za-z0-9_]*\[.*\][ \t]*=/
/[[:alpha:]_][[:alnum:]_]*\[.*\][[:blank:]]*=/ {cnt++}
END{print cnt}
