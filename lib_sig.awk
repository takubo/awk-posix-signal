function kill(sig, pid) {
	print "" | "kill -" sig " " pid
}

function killpg(sig, pid) {
	print "" | "kill -" sig " " -pid
}

function raise(sig) {
	print "" | "kill -" sig " " PROCINFO["pid"]
}

function alarm(sec) {
	if (sec)
		print "" | "(sleep " sec "; kill -ALRM " PROCINFO["pid"] ")"
}

function xalarm(sec, sig, pid) {
	if (sec)
		print "" | "(sleep " sec "; kill -" \
			   (sig == "" ? "ALRM" : sig) " " \
			   (pid == "" ? PROCINFO["pid"] : pid) ")"
}

function setitimer(sec) {
	if (sec)
		print "" | "(while true ; do sleep " sec "; kill -ALRM " PROCINFO["pid"] "; done)"
}

