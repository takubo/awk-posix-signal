#!/usr/local/bin/gawk -f
#
# This program available with gawk-4.0.0 or later.
#

function ALRM_handler(sig)
{
	print "ALRM_handler called."
}

BEGIN {
	extension("./awk-signal.so", "dlload")

	if (!ARGV[1]) {
		signal("ALRM", "ALRM_handler")
	}

	alarm(3)
	pause()

	# if ARGV[1] is false, do not execute this line.
	print "continuing"
}
