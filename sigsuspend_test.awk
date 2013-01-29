#!/usr/local/bin/gawk -f
#
# This program available with gawk-4.0.0 or later.
#

function INT_handler(sig) { print "INT_handler called." }

function USR1_handler(sig) { print "USR1_handler called." }

function USR2_handler(sig) { print "USR2_handler called." }

BEGIN {
	extension("./awk-signal.so", "dlload")

	signal("INT", "INT_handler")
	signal("USR1", "USR1_handler")

	mask["INT"] = 1
	mask["USR1"] = 1

	print "#1"
	sigsuspend(mask)
	print "#2"
}
