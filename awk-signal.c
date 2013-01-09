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







#if 0


int str2sig(const char *str)
{
	int sig = 0;

	if (strlen(str) == 1) {
		switch (str[0]) {
		case '1':
			sig = SIGHUP;
			break;
		case '2':
			sig = SIGINT;
			break;
		case '3':
			sig = SIGQUIT;
			break;
		case '6':
			sig = SIGABRT;
			break;
		case '8':
			sig = SIGFPE;
			break;
		case '9':
			sig = SIGKILL;
			break;
		case '14':
			sig = SIGALRM;
			break;
		case '15':
			sig = SIGTERM;
			break;
		}
	}
}

void str_toupper(char * s)
{
	for (; *s; s++) *s = toupper(*s);
	return;
}

do_kill(int nags)
{
	NODE *tmp;
	pid_t pid;
	int sig;

	if (do_lint && get_curfunc_arg_count() > 2)
		lintwarn("kill: called with too many arguments");

	tmp = (NODE *) get_actual_argument(0, FALSE, FALSE);
	pid = (pid_t) force_number(tmp);

	tmp = (NODE *) get_actual_argument(1, FALSE, FALSE);
	sig = get_sig(tmp);
	if (sig == 0) {
		// TODO
	}

	return make_number((AWKNUM) kill(pid, sig));
}


static NODE *
do_select(int nags)
{
	NODE *tmp, *array, *elm;
	int nfds;
	fd_set rfds, wfds, efds;
	struct timeval timeout;
	int retval;
	struct redirect *rp;
	double integer, point;
	int i, j;
	int fp;

	// TODO lint

	nfds = -1;
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	for (i = 0; i < 2; i++ ) {
		//TODO tmp = (NODE*) get_actual_argument(0 + i, FALSE, FALSE);
		array = get_array_argument(0 + 1, FALSE);
		if ( array == NULL ) { continue; }

		for (j = 0; j < array->array_size; j++) {
			// TODO Scan Array
			elm = array->var_array[j];
			if (elm->type == Node_val) {
				//TODO force_string(tmp = elm->val);
			} else if (elm->type == Node_var ) {
				//TODO force_string(tmp = elm->lnode->val);
			} else {
				// TODO lint
				continue;
			}

			rp = getredirect(tmp->stptr, tmp->stlen);
			if (rp == NULL) {
				if (do_lint) {
					lintwarn("select: `%.*s' is not an open file, pipe or co-process",
						(int) tmp->stlen, tmp->stptr);
				}
			}

			fp = fileno(rp->fp); 

			//TODO ファイルディスクリプタの方向
			switch (i) {
			case 0:
				FD_SET(fp, &rfds);
				break;
			case 1:
				FD_SET(fp, &wfds);
				break;
			case 2:
				FD_SET(fp, &efds);
				break;
			}

			if (fp + 1 > nfds ) { nfds = fp + 1; }
		}
	}

	/*** Analyse Timeout ***/
	/* timeout specified as milli-seconds */

	tmp = (NODE*) get_actual_argument(3, FALSE, FALSE);
	point = modf(force_number(tmp), &integer);
	timeout.tv_sec = (time_t) (integer / 1000);
	timeout.tv_usec = (suseconds_t) (point * 1000);

	retval = select(nfds, &rfds, &wfds, &efds, &timeout);

	if (retval == -1) {
		perror("select()");
		return make_number((AWKNUM) -1);
	} else if (retval == 0) {
		return make_number((AWKNUM) 0);
	} else {
		return make_number((AWKNUM) retval);
	}
}
static NODE *
can_sub(int mode)
{
	NODE *tmp;
	int nfds;
	fd_set fds;
	struct timeval timeout;
	int retval;
	struct redirect *rp;
	double integer, point;


	/*** Analyse Timeout ***/
	/* timeout specified as milli-seconds */

	tmp = (NODE*) get_actual_argument(1, FALSE, FALSE);
	point = modf(force_number(tmp), &integer);
	timeout.tv_sec = (time_t) (integer / 1000);
	timeout.tv_usec = (suseconds_t) (point * 1000);


	/*** Analyse File-descriptor ***/

	tmp = (NODE*) get_actual_argument(0, FALSE, FALSE);
	force_string(tmp);


	rp = getredirect(tmp->stptr, tmp->stlen);

	if (rp == NULL) {	/* no match, return -1 */

		if (do_lint) {
			const char *fn;

			switch (mode) {
			case CAN_READ:
				fn = "can_read";
				break;
			case CAN_WRITE:
				fn = "can_write";
				break;
			case HAS_EXCEPTION:
				fn = "has_excepstion";
				break;
			}
			lintwarn("can_%s: `%.*s' is not an open file, pipe or co-process",
				fn, (int) tmp->stlen, tmp->stptr);
		}

		return make_number((AWKNUM) -1);
	}

	nfds = fileno(rp->fp) + 1;

	FD_ZERO(&fds);
	FD_SET(fileno(rp->fp), &fds);

	//TODO ファイルディスクリプタの方向
	switch (mode) {
	case CAN_READ:
		retval = select(nfds, &fds, NULL, NULL, &timeout);
		break;
	case CAN_WRITE:
		retval = select(nfds, NULL, &fds, NULL, &timeout);
		break;
	case HAS_EXCEPTION:
		retval = select(nfds, NULL, NULL, &fds, &timeout);
		break;
	}

	if (retval == -1) {
		perror("select()");
		return make_number((AWKNUM) -1);
	} else if (retval == 0) {
		return make_number((AWKNUM) 0);
	} else {
		return make_number((AWKNUM) retval);
	}
}

static NODE *
do_can_read(int nags)
{
	if (do_lint && get_curfunc_arg_count() > 2)
		lintwarn("can_write: called with too many arguments");

	return can_sub(CAN_READ);
}

int str2sig(const char *str)
{
	int sig = 0;

// 	str_toupper(str);
// 	if (strncmp(str, "SIG", 3) == 0) {
// 		str += 3;
// 	}

	if (strcmp(str, "HUP") == 0) {
		sig = SIGHUP
	} else if (strcmp(str, "INT") == 0) {
		sig = SIGINT
	} else if (strcmp(str, "QUIT") == 0) {
		sig = SIGQUIT
	} else if (strcmp(str, "ILL") == 0) {
		sig = SIGILL
	} else if (strcmp(str, "ABRT") == 0) {
		sig = SIGABRT
	} else if (strcmp(str, "FPE") == 0) {
		sig = SIGFPE
	} else if (strcmp(str, "KILL") == 0) {
		sig = SIGKILL
	} else if (strcmp(str, "SEGV") == 0) {
		sig = SIGSEGV
	} else if (strcmp(str, "PIPE") == 0) {
		sig = SIGPIPE
	} else if (strcmp(str, "ALRM") == 0) {
		sig = SIGALRM
	} else if (strcmp(str, "TERM") == 0) {
		sig = SIGTERM
	} else if (strcmp(str, "USR1") == 0) {
		sig = SIGUSR1
	} else if (strcmp(str, "USR2") == 0) {
		sig = SIGUSR2
	} else if (strcmp(str, "CHLD") == 0) {
		sig = SIGCHLD
	} else if (strcmp(str, "CONT") == 0) {
		sig = SIGCONT
	} else if (strcmp(str, "STOP") == 0) {
		sig = SIGSTOP
	} else if (strcmp(str, "TSTP") == 0) {
		sig = SIGTSTP
	} else if (strcmp(str, "TTIN") == 0) {
		sig = SIGTTIN
	} else if (strcmp(str, "TTOU") == 0) {
		sig = SIGTTOU
	} else if (strcmp(str, "BUS") == 0) {
		sig = SIGBUS
	} else if (strcmp(str, "POLL") == 0) {
		sig = SIGPOLL
	} else if (strcmp(str, "PROF") == 0) {
		sig = SIGPROF
	} else if (strcmp(str, "SYS") == 0) {
		sig = SIGSYS
	} else if (strcmp(str, "TRAP") == 0) {
		sig = SIGTRAP
	} else if (strcmp(str, "URG") == 0) {
		sig = SIGURG
	} else if (strcmp(str, "VTALRM") == 0) {
		sig = SIGVTALRM
	} else if (strcmp(str, "XCPU") == 0) {
		sig = SIGXCPU
	} else if (strcmp(str, "XFSZ") == 0) {
		sig = SIGXFSZ
	} else if (strcmp(str, "IOT") == 0) {
		sig = SIGIOT
	} else if (strcmp(str, "EMT") == 0) {
		sig = SIGEMT
	} else if (strcmp(str, "STKFLT") == 0) {
		sig = SIGSTKFLT
	} else if (strcmp(str, "IO") == 0) {
		sig = SIGIO
	} else if (strcmp(str, "CLD") == 0) {
		sig = SIGCLD
	} else if (strcmp(str, "PWR") == 0) {
		sig = SIGPWR
	} else if (strcmp(str, "INFO") == 0) {
		sig = SIGINFO
	} else if (strcmp(str, "LOST") == 0) {
		sig = SIGLOST
	} else if (strcmp(str, "WINCH") == 0) {
		sig = SIGWINCH
	} else if (strcmp(str, "UNUSED") == 0) {
		sig = SIGUNUSED
	}

	return sig;
}


#endif
