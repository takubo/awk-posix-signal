#!/usr/local/bin/gawk -f
#
# This program available with gawk-4.0.0 or later.
#

BEGIN {
	extension("./awk-signal.so", "dlload")

	pid = ARGV[1]

	time = 0.5

	kill(pid, "HUP");	system("sleep " time)
	kill(pid, "INT");	system("sleep " time)
	kill(pid, "QUIT");	system("sleep " time)
	kill(pid, "ILL");	system("sleep " time)
	kill(pid, "ABRT");	system("sleep " time)
	kill(pid, "FPE");	system("sleep " time)
	#?kill(pid, "KILL");	system("sleep " time)
	kill(pid, "SEGV");	system("sleep " time)
	kill(pid, "PIPE");	system("sleep " time)
	kill(pid, "ALRM");	system("sleep " time)
	kill(pid, "TERM");	system("sleep " time)
	kill(pid, "USR1");	system("sleep " time)
	kill(pid, "USR2");	system("sleep " time)
	kill(pid, "CHLD");	system("sleep " time)
	kill(pid, "CONT");	system("sleep " time)
	#?kill(pid, "STOP");	system("sleep " time)
	kill(pid, "TSTP");	system("sleep " time)
	kill(pid, "TTIN");	system("sleep " time)
	kill(pid, "TTOU");	system("sleep " time)
	kill(pid, "BUS");	system("sleep " time)
	kill(pid, "POLL");	system("sleep " time)
	kill(pid, "PROF");	system("sleep " time)
	kill(pid, "SYS");	system("sleep " time)
	kill(pid, "TRAP");	system("sleep " time)
	kill(pid, "URG");	system("sleep " time)
	kill(pid, "VTALRM");	system("sleep " time)
	kill(pid, "XCPU");	system("sleep " time)
	kill(pid, "XFSZ");	system("sleep " time)
	kill(pid, "IOT");	system("sleep " time)
	#kill(pid, "EMT");	system("sleep " time)
	kill(pid, "STKFLT");	system("sleep " time)
	kill(pid, "IO");	system("sleep " time)
	kill(pid, "CLD");	system("sleep " time)
	kill(pid, "PWR");	system("sleep " time)
	#kill(pid, "INFO");	system("sleep " time)
	#kill(pid, "LOST");	system("sleep " time)
	kill(pid, "WINCH");	system("sleep " time)
	kill(pid, "UNUSED");	system("sleep " time)

	kill(pid, "STOP");	system("sleep " time)
	kill(pid, "KILL");	system("sleep " time)
}
