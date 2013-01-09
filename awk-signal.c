//#include <stdio.h>
//#include <stdlib.h>


#if defined NO_POSIX_1_2001
#endif

#include <signal.h>
// #include <string.h>
// #include "gawk-4.0.1/awk.h"
#include "awk.h"


int plugin_is_GPL_compatible;


#define SIG_NUM		128
static NODE *user_handler[SIG_NUM];
static struct sigaction sig_act[SIG_NUM];


static INSTRUCTION *code;

static void
handler(int sig)
{
	AWKNUM ret;
	extern int currule;
	extern int exiting;

	code->func_body = user_handler[sig];
	(code + 1)->expr_count = 0;	/* function takes no argument */

#ifdef DBG
	printf("%d %p\n",sig,code->func_body);
#endif

	/* make non-local jumps `next' and `nextfile' fatal in
	 * callback function by setting currule in interpret()
	 * to undefined (0). `exit' is handled in user_func.
	 */

	(code + 1)->inrule = currule;	/* save current rule */
	currule = 0;

	interpret(code);
	if (exiting)	/* do not assume anything about the user-defined function! */
		gawk_exit(exit_val);

	POP_NUMBER(ret);

	currule = (code + 1)->inrule;   /* restore current rule */
	return;
}


int
str2sig(const char *str)
{
	int sig = 0;

	if (strcmp(str, "HUP") == 0) {
		sig = SIGHUP;
	} else if (strcmp(str, "INT") == 0) {
		sig = SIGINT;
	} else if (strcmp(str, "QUIT") == 0) {
		sig = SIGQUIT;
	} else if (strcmp(str, "ILL") == 0) {
		sig = SIGILL;
	} else if (strcmp(str, "ABRT") == 0) {
		sig = SIGABRT;
	} else if (strcmp(str, "FPE") == 0) {
		sig = SIGFPE;
	} else if (strcmp(str, "KILL") == 0) {
		sig = SIGKILL;
	} else if (strcmp(str, "SEGV") == 0) {
		sig = SIGSEGV;
	} else if (strcmp(str, "PIPE") == 0) {
		sig = SIGPIPE;
	} else if (strcmp(str, "ALRM") == 0) {
		sig = SIGALRM;
	} else if (strcmp(str, "TERM") == 0) {
		sig = SIGTERM;
	} else if (strcmp(str, "USR1") == 0) {
		sig = SIGUSR1;
	} else if (strcmp(str, "USR2") == 0) {
		sig = SIGUSR2;
	} else if (strcmp(str, "CHLD") == 0) {
		sig = SIGCHLD;
	} else if (strcmp(str, "CONT") == 0) {
		sig = SIGCONT;
	} else if (strcmp(str, "STOP") == 0) {
		sig = SIGSTOP;
	} else if (strcmp(str, "TSTP") == 0) {
		sig = SIGTSTP;
	} else if (strcmp(str, "TTIN") == 0) {
		sig = SIGTTIN;
	} else if (strcmp(str, "TTOU") == 0) {
		sig = SIGTTOU;
	} else if (strcmp(str, "BUS") == 0) {
		sig = SIGBUS;
	} else if (strcmp(str, "POLL") == 0) {
		sig = SIGPOLL;
	} else if (strcmp(str, "PROF") == 0) {
		sig = SIGPROF;
	} else if (strcmp(str, "SYS") == 0) {
		sig = SIGSYS;
	} else if (strcmp(str, "TRAP") == 0) {
		sig = SIGTRAP;
	} else if (strcmp(str, "URG") == 0) {
		sig = SIGURG;
	} else if (strcmp(str, "VTALRM") == 0) {
		sig = SIGVTALRM;
	} else if (strcmp(str, "XCPU") == 0) {
		sig = SIGXCPU;
	} else if (strcmp(str, "XFSZ") == 0) {
		sig = SIGXFSZ;
	} else if (strcmp(str, "IOT") == 0) {
		sig = SIGIOT;
	//} else if (strcmp(str, "EMT") == 0) {
		//sig = SIGEMT;
	} else if (strcmp(str, "STKFLT") == 0) {
		sig = SIGSTKFLT;
	} else if (strcmp(str, "IO") == 0) {
		sig = SIGIO;
	} else if (strcmp(str, "CLD") == 0) {
		sig = SIGCLD;
	} else if (strcmp(str, "PWR") == 0) {
		sig = SIGPWR;
	//} else if (strcmp(str, "INFO") == 0) {
		//sig = SIGINFO;
	//} else if (strcmp(str, "LOST") == 0) {
		//sig = SIGLOST;
	} else if (strcmp(str, "WINCH") == 0) {
		sig = SIGWINCH;
	} else if (strcmp(str, "UNUSED") == 0) {
		sig = SIGUNUSED;
	}

	return sig;
}

int
num2sig(int num)
{
	int sig = 0;

	switch (num) {
		case SIGHUP:
		case SIGINT:
		case SIGQUIT:
		case SIGILL:
		case SIGABRT:
		case SIGFPE:
		case SIGKILL:
		case SIGSEGV:
		case SIGPIPE:
		case SIGALRM:
		case SIGTERM:
		case SIGUSR1:
		case SIGUSR2:
		case SIGCHLD:
		case SIGCONT:
		case SIGSTOP:
		case SIGTSTP:
		case SIGTTIN:
		case SIGTTOU:
		case SIGBUS:
		case SIGPOLL:
		case SIGPROF:
		case SIGSYS:
		case SIGTRAP:
		case SIGURG:
		case SIGVTALRM:
		case SIGXCPU:
		case SIGXFSZ:
		//?case SIGIOT:
		//case SIGEMT:
		case SIGSTKFLT:
		//?case SIGIO:
		//?case SIGCLD:
		case SIGPWR:
		//case SIGINFO:
		//case SIGLOST:
		case SIGWINCH:
		//?case SIGUNUSED:
			sig = num;
	}

	return sig;
}

int
get_sig(NODE *tmp)
{
	int sig;

	force_string(tmp);
	sig = str2sig(tmp->stptr);

	if (sig == 0) {
		sig = num2sig((int) force_number(tmp));
	}

	if (sig == 0) {
		force_string(tmp);
		fatal(_("Signal `%s' is not defined"), tmp->stptr);
	}

	return sig;
}

static NODE *
kill_killpg(int (*func)(int, int))
{
	NODE *tmp;
	pid_t pid_prg;
	int sig;

	tmp = (NODE *) get_scalar_argument(0, FALSE);
	pid_prg = (pid_t) force_number(tmp);

	tmp = (NODE *) get_scalar_argument(1, FALSE);
	sig = get_sig(tmp);

	return make_number((AWKNUM) func(pid_prg, sig));
}

static NODE *
do_kill(int nargs)
{
	if (do_lint && get_curfunc_arg_count() > 2)
		lintwarn("kill: called with too many arguments");

	return kill_killpg(kill);
}

static NODE *
do_killpg(int nargs)
{
	if (do_lint && get_curfunc_arg_count() > 2)
		lintwarn("killpg: called with too many arguments");

	return kill_killpg(killpg);
}

static NODE *
do_raise(int nargs)
{
	NODE *tmp;
	int sig;

	if (do_lint && get_curfunc_arg_count() > 1)
		lintwarn("raise: called with too many arguments");

	tmp = (NODE *) get_scalar_argument(0, FALSE);
	sig = get_sig(tmp);

	return make_number((AWKNUM) raise(sig));
}

static NODE *
do_sigact(int nargs)
{
	NODE *tmp;
	int sig;
	NODE *fnc_ptr;
	struct sigaction *saptr;

	if (do_lint && get_curfunc_arg_count() > 2)
		lintwarn("sigact: called with too many arguments");

	tmp = (NODE *) get_scalar_argument(0, FALSE);
	sig = get_sig(tmp);

	tmp = (NODE *) get_scalar_argument(1, FALSE);
	force_string(tmp);

	fnc_ptr = lookup(tmp->stptr);
	if (fnc_ptr == NULL || fnc_ptr->type != Node_func)
		fatal(_("Callback function `%s' is not defined"), tmp->stptr);

	user_handler[sig] = fnc_ptr;

	saptr = &sig_act[sig];
	saptr->sa_handler = handler;
	saptr->sa_flags |= SA_RESTART;    /* システムコールが中止しない */

#ifdef DBG
//printf("%d %p\n",sig,fnc_ptr);
#endif

	return make_number((AWKNUM) sigaction(sig, saptr, NULL));
}

static NODE *
do_sigaction(int nargs)
{
	if (do_lint && get_curfunc_arg_count() > 4)
		lintwarn("sigaction: called with too many arguments");

	return make_number((AWKNUM) pause());
}

static NODE *
do_pause(int nargs)
{
	if (do_lint && get_curfunc_arg_count() > 0)
		lintwarn("pause: called with too many arguments");

	return make_number((AWKNUM) pause());
}

NODE *
dlload(NODE *tree, void *dl)
{
	int i;

	make_builtin("kill", do_kill, 2);
	make_builtin("killpg", do_killpg, 2);
	make_builtin("raise", do_raise, 1);
	make_builtin("sigact", do_sigact, 2);
	make_builtin("sigaction", do_sigaction, 4);
	make_builtin("pause", do_pause, 4);

	/* make function call instructions */
	code = bcalloc(Op_func_call, 2, 0);
	code->func_name = NULL;		/* not needed, func_body will assign */
	code->nexti = bcalloc(Op_stop, 1, 0);

	for (i = 0; i < SIG_NUM; i++) {
		// user_handler[i] = NULL;
		memset(&sig_act[i], 0, sizeof(struct sigaction));
	}

	return make_number((AWKNUM) 0);
}
