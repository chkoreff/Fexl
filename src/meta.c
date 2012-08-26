#include <string.h> /* strlen */
#include <stdlib.h> /* exit */
#include <sys/resource.h> /* setrlimit, getrlimit */
#include <sys/types.h> /* pid_t */
#include <sys/wait.h> /* wait */
#include <unistd.h> /* fork */
#include "base.h"
#include "memory.h"
#include "value.h"
#include "long.h"
#include "run.h"
#include "string.h"

/* (argc next) = next val */
void reduce_argc(value f)
	{
	replace_apply(f,f->R,Qlong(main_argc));
	}

/* (argv i next) = next val */
static void reduce2_argv(value f)
	{
	long i = get_long(arg(type_long,f->L->R));
	replace_apply(f, f->R, Qstring(i < main_argc ? main_argv[i] : ""));
	}

void reduce_argv(value f)
	{
	f->T = reduce2_argv;
	}

/* (envc next) = next val */
void reduce_envc(value f)
	{
	replace_apply(f, f->R, Qlong(main_envc()));
	}

/* (envp i next) = next val */
static void reduce2_envp(value f)
	{
	long i = get_long(arg(type_long,f->L->R));
	replace_apply(f, f->R, Qstring(i < main_envc() ? main_envp[i] : ""));
	}

void reduce_envp(value f)
	{
	f->T = reduce2_envp;
	}

/* (exit status) exits immediately with the given status. */
void reduce_exit(value f)
	{
	long status = get_long(arg(type_long,f->R));
	exit(status);
	}

/* (base_path next) = (next path) where path is the base path of the current
Fexl executable. */
void reduce_base_path(value f)
	{
	replace_apply(f, f->R, Qstring(base));
	}

/* (fork next) = (next pid), where pid is the result of calling fork(2). */
void reduce_fork(value f)
	{
	replace_apply(f, f->R, Qlong(fork()));
	}

/* (wait next) = (next pid status) */
void reduce_wait(value f)
	{
	int status;
	pid_t pid = wait(&status);
	replace_apply(f, A(f->R,Qlong(pid)), Qlong(status));
	}

value const_RLIMIT_AS(void) { return Qlong(RLIMIT_AS); }
value const_RLIMIT_CORE(void) { return Qlong(RLIMIT_CORE); }
value const_RLIMIT_CPU(void) { return Qlong(RLIMIT_CPU); }
value const_RLIMIT_DATA(void) { return Qlong(RLIMIT_DATA); }
value const_RLIMIT_FSIZE(void) { return Qlong(RLIMIT_FSIZE); }
value const_RLIMIT_LOCKS(void) { return Qlong(RLIMIT_LOCKS); }
value const_RLIMIT_MEMLOCK(void) { return Qlong(RLIMIT_MEMLOCK); }
value const_RLIMIT_MSGQUEUE(void) { return Qlong(RLIMIT_MSGQUEUE); }
value const_RLIMIT_NICE(void) { return Qlong(RLIMIT_NICE); }
value const_RLIMIT_NOFILE(void) { return Qlong(RLIMIT_NOFILE); }
value const_RLIMIT_NPROC(void) { return Qlong(RLIMIT_NPROC); }
value const_RLIMIT_RSS(void) { return Qlong(RLIMIT_RSS); }
value const_RLIMIT_RTPRIO(void) { return Qlong(RLIMIT_RTPRIO); }
value const_RLIMIT_RTTIME(void) { return Qlong(RLIMIT_RTTIME); }
value const_RLIMIT_SIGPENDING(void) { return Qlong(RLIMIT_SIGPENDING); }
value const_RLIMIT_STACK(void) { return Qlong(RLIMIT_STACK); }

/* (setrlimit resource soft hard next) = (next status) */
static void reduce4_setrlimit(value f)
	{
	long resource = get_long(arg(type_long,f->L->L->L->R));
	struct rlimit rlim;
	rlim.rlim_cur = get_long(arg(type_long,f->L->L->R));
	rlim.rlim_max = get_long(arg(type_long,f->L->R));

	int status = setrlimit(resource, &rlim);
	replace_apply(f, f->R, Qlong(status));
	}

static void reduce3_setrlimit(value f)
	{
	f->T = reduce4_setrlimit;
	}

static void reduce2_setrlimit(value f)
	{
	f->T = reduce3_setrlimit;
	}

void reduce_setrlimit(value f)
	{
	f->T = reduce2_setrlimit;
	}

/* (getrlimit resource next) = (next status soft hard) */
static void reduce2_getrlimit(value f)
	{
	long resource = get_long(arg(type_long,f->L->R));
	struct rlimit rlim;
	int status = getrlimit(resource, &rlim);

	replace_apply(f, A(A(f->R,Qlong(status)),
		Qlong(rlim.rlim_cur)),
		Qlong(rlim.rlim_max));
	}

void reduce_getrlimit(value f)
	{
	f->T = reduce2_getrlimit;
	}

/* LATER getrusage */
