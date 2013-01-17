#!/usr/local/bin/gawk -f
#
# This program available with gawk-4.0.0 or later.
#

function HUP_handler(sig)
{
	print "HUP_handler called."
}

function INT_handler(sig)
{
	print "INT_handler called."
}

function QUIT_handler(sig)
{
	print "QUIT_handler called."
}

function ILL_handler(sig)
{
	print "ILL_handler called."
}

function ABRT_handler(sig)
{
	print "ABRT_handler called."
}

function FPE_handler(sig)
{
	print "FPE_handler called."
}

function KILL_handler(sig)
{
	print "KILL_handler called."
}

function SEGV_handler(sig)
{
	print "SEGV_handler called."
}

function PIPE_handler(sig)
{
	print "PIPE_handler called."
}

function ALRM_handler(sig)
{
	print "ALRM_handler called."
}

function TERM_handler(sig)
{
	print "TERM_handler called."
}

function USR1_handler(sig)
{
	print "USR1_handler called."
}

function USR2_handler(sig)
{
	print "USR2_handler called."
}

function CHLD_handler(sig)
{
	print "CHLD_handler called."
}

function CONT_handler(sig)
{
	print "CONT_handler called."
}

function STOP_handler(sig)
{
	print "STOP_handler called."
}

function TSTP_handler(sig)
{
	print "TSTP_handler called."
}

function TTIN_handler(sig)
{
	print "TTIN_handler called."
}

function TTOU_handler(sig)
{
	print "TTOU_handler called."
}

function BUS_handler(sig)
{
	print "BUS_handler called."
}

function POLL_handler(sig)
{
	print "POLL_handler called."
}

function PROF_handler(sig)
{
	print "PROF_handler called."
}

function SYS_handler(sig)
{
	print "SYS_handler called."
}

function TRAP_handler(sig)
{
	print "TRAP_handler called."
}

function URG_handler(sig)
{
	print "URG_handler called."
}

function VTALRM_handler(sig)
{
	print "VTALRM_handler called."
}

function XCPU_handler(sig)
{
	print "XCPU_handler called."
}

function XFSZ_handler(sig)
{
	print "XFSZ_handler called."
}

function IOT_handler(sig)
{
	print "IOT_handler called."
}

function EMT_handler(sig)
{
	print "EMT_handler called."
}

function STKFLT_handler(sig)
{
	print "STKFLT_handler called."
}

function IO_handler(sig)
{
	print "IO_handler called."
}

function CLD_handler(sig)
{
	print "CLD_handler called."
}

function PWR_handler(sig)
{
	print "PWR_handler called."
}

function INFO_handler(sig)
{
	print "INFO_handler called."
}

function LOST_handler(sig)
{
	print "LOST_handler called."
}

function WINCH_handler(sig)
{
	print "WINCH_handler called."
}

function UNUSED_handler(sig)
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
	sigact("STOP", "STOP_handler")
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
