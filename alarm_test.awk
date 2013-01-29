#!/usr/local/bin/gawk -f
#
# This program available with gawk-4.0.0 or later.
#

function ALRM_handler(sig)
{
	print "ALRM_handler called."
	alarm(1)
}

BEGIN {
	extension("./awk-signal.so", "dlload")

	signal("ALRM", "ALRM_handler")

	alarm(1)
	for (i = 0; i < 20; i++) {
		print "looping..."
		system("sleep 0.4")
	}
}
