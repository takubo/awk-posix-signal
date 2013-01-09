#!/usr/local/bin/gawk -f
#
# This program available with gawk-4.0.0 or later.
#

function HUP_handler()
{
	print "HUP_handler called."
}

function INT_handler()
{
	print "INT_handler called."
}

function QUIT_handler()
{
	print "QUIT_handler called."
}

function ILL_handler()
{
	print "ILL_handler called."
}

function ABRT_handler()
{
	print "ABRT_handler called."
}

function FPE_handler()
{
	print "FPE_handler called."
}

function KILL_handler()
{
	print "KILL_handler called."
}

function SEGV_handler()
{
	print "SEGV_handler called."
}

function PIPE_handler()
{
	print "PIPE_handler called."
}

function ALRM_handler()
{
	print "ALRM_handler called."
}

function TERM_handler()
{
	print "TERM_handler called."
}

function USR1_handler()
{
	print "USR1_handler called."
}

function USR2_handler()
{
	print "USR2_handler called."
}

function CHLD_handler()
{
	print "CHLD_handler called."
}

function CONT_handler()
{
	print "CONT_handler called."
}

function STOP_handler()
{
	print "STOP_handler called."
}

function TSTP_handler()
{
	print "TSTP_handler called."
}

function TTIN_handler()
{
	print "TTIN_handler called."
}

function TTOU_handler()
{
	print "TTOU_handler called."
}

function BUS_handler()
{
	print "BUS_handler called."
}

function POLL_handler()
{
	print "POLL_handler called."
}

function PROF_handler()
{
	print "PROF_handler called."
}

function SYS_handler()
{
	print "SYS_handler called."
}

function TRAP_handler()
{
	print "TRAP_handler called."
}

function URG_handler()
{
	print "URG_handler called."
}

function VTALRM_handler()
{
	print "VTALRM_handler called."
}

function XCPU_handler()
{
	print "XCPU_handler called."
}

function XFSZ_handler()
{
	print "XFSZ_handler called."
}

function IOT_handler()
{
	print "IOT_handler called."
}

function EMT_handler()
{
	print "EMT_handler called."
}

function STKFLT_handler()
{
	print "STKFLT_handler called."
}

function IO_handler()
{
	print "IO_handler called."
}

function CLD_handler()
{
	print "CLD_handler called."
}

function PWR_handler()
{
	print "PWR_handler called."
}

function INFO_handler()
{
	print "INFO_handler called."
}

function LOST_handler()
{
	print "LOST_handler called."
}

function WINCH_handler()
{
	print "WINCH_handler called."
}

function UNUSED_handler()
{
	print "UNUSED_handler called."
}
BEGIN {
	extension("./awk-signal.so", "dlload")

	sigact("HUP", "HUP_handler")
	sigact("INT", "INT_handler")
	sigact("QUIT", "QUIT_handler")
	sigact("ILL", "ILL_handler")
	sigact("ABRT", "ABRT_handler")
	sigact("FPE", "FPE_handler")
	sigact("KILL", "KILL_handler")
	sigact("SEGV", "SEGV_handler")
	sigact("PIPE", "PIPE_handler")
	sigact("ALRM", "ALRM_handler")
	sigact("TERM", "TERM_handler")
	sigact("USR1", "USR1_handler")
	sigact("USR2", "USR2_handler")
	sigact("CHLD", "CHLD_handler")
	sigact("CONT", "CONT_handler")
	#?sigact("STOP", "STOP_handler")
	sigact("TSTP", "TSTP_handler")
	sigact("TTIN", "TTIN_handler")
	sigact("TTOU", "TTOU_handler")
	sigact("BUS", "BUS_handler")
	sigact("POLL", "POLL_handler")
	sigact("PROF", "PROF_handler")
	sigact("SYS", "SYS_handler")
	sigact("TRAP", "TRAP_handler")
	sigact("URG", "URG_handler")
	sigact("VTALRM", "VTALRM_handler")
	sigact("XCPU", "XCPU_handler")
	sigact("XFSZ", "XFSZ_handler")
	sigact("IOT", "IOT_handler")
	#sigact("EMT", "EMT_handler")
	sigact("STKFLT", "STKFLT_handler")
	sigact("IO", "IO_handler")
	sigact("CLD", "CLD_handler")
	sigact("PWR", "PWR_handler")
	#sigact("INFO", "INFO_handler")
	#sigact("LOST", "LOST_handler")
	sigact("WINCH", "WINCH_handler")
	sigact("UNUSED", "UNUSED_handler")

	for ( ; ; );
	#for ( ; ; ){print "##";system("sleep 0.1")}
}
