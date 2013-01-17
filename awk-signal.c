#ifdef NO_POSIX_1_2001
#endif

#include <signal.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "awk.h"


int plugin_is_GPL_compatible;


typedef struct {
	int sig_no;
	const char *sig_name;
	NODE *user_handler;
} SigTable;

SigTable sig_table[] = {
  /* POSIX.1-1990 */
	{ SIGHUP,	"HUP",	NULL },
	{ SIGINT,	"INT",	NULL },
	{ SIGQUIT,	"QUIT",	NULL },
	{ SIGILL,	"ILL",	NULL },
	{ SIGABRT,	"ABRT",	NULL },
	{ SIGFPE,	"FPE",	NULL },
	{ SIGKILL,	"KILL",	NULL },
	{ SIGSEGV,	"SEGV",	NULL },
	{ SIGPIPE,	"PIPE",	NULL },
	{ SIGALRM,	"ALRM",	NULL },
	{ SIGTERM,	"TERM",	NULL },
	{ SIGUSR1,	"USR1",	NULL },
	{ SIGUSR2,	"USR2",	NULL },
	{ SIGCHLD,	"CHLD",	NULL },
	{ SIGCONT,	"CONT",	NULL },
	{ SIGSTOP,	"STOP",	NULL },
	{ SIGTSTP,	"TSTP",	NULL },
	{ SIGTTIN,	"TTIN",	NULL },
	{ SIGTTOU,	"TTOU",	NULL },
  /* SUSv2 & POSIX.1-2001 */
	{ SIGBUS,	"BUS",	NULL },
	{ SIGPOLL,	"POLL",	NULL },
	{ SIGPROF,	"PROF",	NULL },
	{ SIGSYS,	"SYS",	NULL },
	{ SIGTRAP,	"TRAP",	NULL },
	{ SIGURG,	"URG",	NULL },
	{ SIGVTALRM,	"VTALRM",	NULL },
	{ SIGXCPU,	"XCPU",	NULL },
	{ SIGXFSZ,	"XFSZ",	NULL },
  /* Others */
// ?	{ SIGIOT,	"IOT",	NULL },	same ABRT (BSD)
//	{ SIGEMT,	"EMT",	NULL },
	{ SIGSTKFLT,	"STKFLT",	NULL },
	{ SIGIO,	"IO",	NULL },
// ?	{ SIGCLD,	"CLD",	NULL },	same CHLD
	{ SIGPWR,	"PWR",	NULL },
//	{ SIGINFO,	"INFO",	NULL },
//	{ SIGLOST,	"LOST",	NULL },
	{ SIGWINCH,	"WINCH",	NULL },
	{ SIGUNUSED,	"UNUSED",	NULL },
  /* pid check only */
	{ 0,	"NULL",	NULL },
};


static void handler(int signo);
static SigTable * sig2ptr(int sig);
static int str2sig(const char *str);
static int num2sig(int num);
static int get_signo(NODE *tmp);
static int ana_mask(sigset_t *mask, int arg_number);
static int ret_mask(sigset_t *mask, int arg_number);
static NODE * kill_killpg(int (*func)(int, int));
static NODE * do_kill(int nargs);
static NODE * do_killpg(int nargs);
static NODE * do_raise(int nargs);
static NODE * do_signal(int nargs);
static int ana_flags(int arg_number);
static int str2flag(const char *str);
static NODE * do_sigprocmask(int nargs);
static int str2how(const char *str);
static NODE * do_sigpending(int nargs);
static NODE * do_pause(int nargs);
static NODE * do_sigsuspend(int nargs);
static NODE * do_alarm(int nargs);
NODE * dlload(NODE *tree, void *dl);


static void
handler(int signo)
{
	SigTable *sig_tbl;
	AWKNUM ret;
	static INSTRUCTION *code;
	extern int currule;
	extern int exiting;

	sig_tbl = sig2ptr(signo);

	/* make function call instructions */
	code = bcalloc(Op_func_call, 2, 0);
	code->func_name = NULL;		/* not needed, func_body will assign */
	code->nexti = bcalloc(Op_stop, 1, 0);

	code->func_body = sig_tbl->user_handler;
	(code + 1)->expr_count = 1;	/* function takes one argument */

	/* make non-local jumps `next' and `nextfile' fatal in
	 * callback function by setting currule in interpret()
	 * to undefined (0). `exit' is handled in user_func.
	 */

	(code + 1)->inrule = currule;	/* save current rule */
	currule = 0;

	PUSH(make_number((AWKNUM) signo));

	interpret(code);
	if (exiting)	/* do not assume anything about the user-defined function! */
		gawk_exit(exit_val);

	POP_NUMBER(ret);

	currule = (code + 1)->inrule;   /* restore current rule */
	bcfree(code->nexti);            /* Op_stop */
	bcfree(code);                   /* Op_func_call */

	return;
}

static SigTable *
sig2ptr(int sig)
{
	int i;

	for (i = 0; i < sizeof(sig_table) / sizeof(SigTable); i++) {
		if (sig == sig_table[i].sig_no) {
			return &sig_table[i];
		}
	}

	return NULL;
}

static int
str2sig(const char *str)
{
	int i;
	int sig = 0;

	if (strncmp(str, "SIG", 3) == 0) {
		str += 3;
	}

	for (i = 0; i < sizeof(sig_table) / sizeof(SigTable); i++) {
		if (strcmp(str, sig_table[i].sig_name) == 0) {
			sig = sig_table[i].sig_no;
			return sig;
		}
	}

	return sig;
}

static int
num2sig(int num)
{
	int i;
	int sig = 0;

	for (i = 0; i < sizeof(sig_table) / sizeof(SigTable); i++) {
		if (num == sig_table[i].sig_no) {
			sig = sig_table[i].sig_no;
			return sig;
		}
	}

	return sig;
}

static int
get_signo(NODE *tmp)
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

static int
ana_mask(sigset_t *mask, int arg_number)
{
	NODE *tmp, *array, *elm, *name, *value;
	int i;
	int sig;

	sigemptyset(mask);

	array = (NODE *) get_array_argument(arg_number, FALSE);
	if ( array != NULL ) {
		for (i = 0; i < array->array_size; i++) {
			for (elm = array->var_array[i]; elm != NULL; elm = elm->ahnext) {
				sig = str2sig(elm->hname);
				if (sig) {
					value = elm->hvalue;
					force_string(value);
					if (value->stptr[0] != '0' && value->stptr[0] != '\0') {
						sigaddset(mask, sig);
					}
				}
			}
		}
	} else {
		tmp = (NODE *) get_scalar_argument(arg_number, FALSE);
		if (strcmp(tmp->stptr, "@all") == 0) {	// toupper TODO	@FILL
			sigfillset(mask);
		} else if (strcmp(tmp->stptr, "@empty") == 0) {	// TODO
			sigemptyset(mask);
		} else {
			
		}
	}

	return 0;
}

static int
ret_mask(sigset_t *mask, int arg_number)
{
	// TODO;
	NODE *tmp, *array, *elm, *value;
	int i;
	int sig;

	array = (NODE *) get_array_argument(arg_number, FALSE);
	if ( array != NULL ) {
#if 0
		for (i = 0; i < array->array_size; i++) {
			for (elm = array->var_array[i]; elm != NULL; elm = elm->ahnext) {
				name = elm->hname;
				force_string(name);
				sig = str2sig(name->stptr);
				if (sig) {
					value = elm->hvalue;
					force_string(value);
					if (value->stptr[0] != '0' && value->stptr[0] != '\0') {
						sigaddset(mask, sig);
					}
				}
			}
		}
#endif
	} else {
		tmp = (NODE *) get_scalar_argument(arg_number, FALSE);
	}

	return 0;
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
	sig = get_signo(tmp);

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
	sig = get_signo(tmp);

	return make_number((AWKNUM) raise(sig));
}

static NODE *
do_signal(int nargs)
{
	NODE *tmp;
	const char *str;
	int sig;
	NODE *fnc_ptr;
	SigTable *sig_tbl;
	struct sigaction sig_act;

	if (do_lint && get_curfunc_arg_count() > 4)
		lintwarn("signal: called with too many arguments");

	memset(&sig_act, 0, sizeof(struct sigaction));

	/* signal number */
	tmp = (NODE *) get_scalar_argument(0, FALSE);
	sig = get_signo(tmp);

	/* signal handler */
	tmp = (NODE *) get_scalar_argument(1, FALSE);
	force_string(tmp);
	str = tmp->stptr;

	sig_tbl = sig2ptr(sig);

	if (str[0] == '@') {
		str++;	/* advance '@' */
		if (strncmp(str, "SIG", 3) == 0) {
			str += 3;
		}
		if (strncmp(str, "_", 1) == 0) {
			str += 1;
		}

		if (strcmp(str, "DFL") == 0) {
			sig_act.sa_handler = SIG_DFL;
		} else if (strcmp(str, "IGN") == 0) {
			sig_act.sa_handler = SIG_IGN;
		} else {
			//TODO sig_act.sa_handler = SIG_IGN; fatal
		}
		sig_tbl->user_handler = NULL;
	} else {
		fnc_ptr = lookup(str);
		sig_tbl->user_handler = fnc_ptr;
		sig_act.sa_handler = handler;
	}

	if (fnc_ptr == NULL || fnc_ptr->type != Node_func)
		fatal(_("Callback function `%s' is not defined"), tmp->stptr);

	/* mask */
	if (get_curfunc_arg_count() >= 3)
		ana_mask(&sig_act.sa_mask, 2);
	else
		sigfillset(&sig_act.sa_mask);

	/* flags */
	if (get_curfunc_arg_count() >= 4)
		sig_act.sa_flags = ana_flags(3);
	else
		sig_act.sa_flags |= SA_RESTART;    /* システムコールが中止しない */

	return make_number((AWKNUM) sigaction(sig, &sig_act, NULL));
}

static int
ana_flags(int arg_number)
{
	NODE *tmp, *array, *elm, *value;
	int i;
	int flag;
	int flags = 0;

	array = (NODE *) get_array_argument(arg_number, FALSE);
	if ( array != NULL ) {
		for (i = 0; i < array->array_size; i++) {
			for (elm = array->var_array[i]; elm != NULL; elm = elm->ahnext) {
				flag = str2flag(elm->hname);
				if (flag) {
					value = elm->hvalue;
					force_string(value);
					if (value->stptr[0] != '0' && value->stptr[0] != '\0') {
						flags |= flag;
					}
				}
			}
		}
	} else {
		tmp = (NODE *) get_scalar_argument(arg_number, FALSE);
		if (tmp->stptr[0] == '\0') {
			flags |= SA_RESTART;    /* システムコールが中止しない */
		} else {
			//TODO;
		}
	}

	return 0;
}

static int
str2flag(const char *str)
{
	int flag;

	if (strncmp(str, "SA_", 3) == 0) {
		str += 3;
	}

	if (strcmp(str, "NOCLDSTOP") == 0) {
		flag = SA_NOCLDSTOP;
	} else if (strcmp(str, "NOCLDWAIT") == 0) {
		flag = SA_NOCLDWAIT;
	} else if (strcmp(str, "NODEFER") == 0) {
		flag = SA_NODEFER;
	} else if (strcmp(str, "RESTART") == 0) {
		flag = SA_RESTART;
	} else if (strcmp(str, "RESETHAND") == 0) {
		flag = SA_RESETHAND;
	} else {
		flag = 0;
	}

	return flag;
}

static NODE *
do_sigprocmask(int nargs)
{
	NODE *tmp;
	int how;
	sigset_t mask;
	sigset_t old;
	int ret;

	if (do_lint && get_curfunc_arg_count() > 1)
		lintwarn("sigsuspend: called with too many arguments");

	tmp = (NODE *) get_scalar_argument(0, FALSE);
	force_string(tmp);
	how = str2how(tmp->stptr);

	ana_mask(&mask, 1);

	ret = sigprocmask(how, &mask, &old);

	ret_mask(&mask, 2);

	return make_number((AWKNUM) ret);
}

static int
str2how(const char *str)
{
	int how;

	if (strncmp(str, "SIG", 3) == 0) {
		str += 3;
	}
	if (strncmp(str, "_", 1) == 0) {
		str += 1;
	}

	if (strcmp(str, "BLOCK") == 0) {
		how = SIG_BLOCK;
	} else if (strcmp(str, "UNBLOCK") == 0) {
		how = SIG_UNBLOCK;
	} else if (strcmp(str, "SETMASK") == 0) {
		how = SIG_SETMASK;
	}

	return how;
}

static NODE *
do_sigpending(int nargs)
{
	sigset_t mask;

	if (do_lint && get_curfunc_arg_count() > 1)
		lintwarn("sigpending: called with too many arguments");

	sigpending(&mask);
	ret_mask(&mask, 0);

	return make_number((AWKNUM) 0);
}

static NODE *
do_pause(int nargs)
{
	if (do_lint && get_curfunc_arg_count() > 0)
		lintwarn("pause: called with too many arguments");

	return make_number((AWKNUM) pause());
}

static NODE *
do_sigsuspend(int nargs)
{
	sigset_t mask;

	if (do_lint && get_curfunc_arg_count() > 1)
		lintwarn("sigsuspend: called with too many arguments");

	ana_mask(&mask, 0);

	return make_number((AWKNUM) sigsuspend(&mask));
}

static NODE *
do_alarm(int nargs)
{
	NODE *tmp;
	unsigned int second;

	if (do_lint && get_curfunc_arg_count() > 1)
		lintwarn("alarm: called with too many arguments");

	tmp = (NODE *) get_scalar_argument(0, FALSE);
	second = (unsigned int) force_number(tmp);

	return make_number((AWKNUM) alarm(second));
}

NODE *
dlload(NODE *tree, void *dl)
{
	make_builtin("kill", do_kill, 2);
	make_builtin("killpg", do_killpg, 2);
	make_builtin("raise", do_raise, 1);

	make_builtin("signal", do_signal, 4);

	//make_builtin("sigaddset", do_sigaddset, 2);
	//make_builtin("sigdelset", do_sigdelset, 2);
	//make_builtin("sigismember", do_ismember, 2);
	//make_builtin("sigfillset", do_sigfillset, 1);
	//make_builtin("sigemptyset", do_sigemptyset, 1);

	make_builtin("sigprocmask", do_sigprocmask, 3);
	make_builtin("sigpending", do_sigpending, 1);

	make_builtin("pause", do_pause, 0);
	make_builtin("sigsuspend", do_sigsuspend, 1);

	make_builtin("alarm", do_alarm, 1);

	return make_number((AWKNUM) 0);
}
