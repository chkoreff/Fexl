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
value fexl_argc(value f)
	{
	if (!f->L) return 0;
	return A(f->R,Qlong(main_argc));
	}

/* (argv i next) = next val */
value fexl_argv(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value x = f->L->R;
	if (!arg(type_long,x)) return 0;

	long i = get_long(x);
	return A(f->R,Qstring(i < main_argc ? main_argv[i] : ""));
	}

/* (envc next) = next val */
value fexl_envc(value f)
	{
	if (!f->L) return 0;
	return A(f->R,Qlong(main_envc()));
	}

/* (envp i next) = next val */
value fexl_envp(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value x = f->L->R;
	if (!arg(type_long,x)) return 0;

	long i = get_long(x);
	return A(f->R,Qstring(i < main_envc() ? main_envp[i] : ""));
	}

/* (exit status) exits immediately with the given status. */
value fexl_exit(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value x = f->L->R;
	if (!arg(type_long,x)) return 0;

	long status = get_long(x);
	exit(status);
	return f->R; /* never reached */
	}

/* (base_path next) = (next path) where path is the base path of the current
Fexl executable. */
value fexl_base_path(value f)
	{
	if (!f->L) return 0;
	return A(f->R,Qstring(base));
	}

/* (fork next) = (next pid), where pid is the result of calling fork(2). */
value fexl_fork(value f)
	{
	if (!f->L) return 0;
	return A(f->R,Qlong(fork()));
	}

/* (wait next) = (next pid status) */
value fexl_wait(value f)
	{
	if (!f->L) return 0;
	int status;
	pid_t pid = wait(&status);
	return A(A(f->R,Qlong(pid)),Qlong(status));
	}

value fexl_RLIMIT_AS(value f)         { return Qlong(RLIMIT_AS); }
value fexl_RLIMIT_CORE(value f)       { return Qlong(RLIMIT_CORE); }
value fexl_RLIMIT_CPU(value f)        { return Qlong(RLIMIT_CPU); }
value fexl_RLIMIT_DATA(value f)       { return Qlong(RLIMIT_DATA); }
value fexl_RLIMIT_FSIZE(value f)      { return Qlong(RLIMIT_FSIZE); }
value fexl_RLIMIT_LOCKS(value f)      { return Qlong(RLIMIT_LOCKS); }
value fexl_RLIMIT_MEMLOCK(value f)    { return Qlong(RLIMIT_MEMLOCK); }
value fexl_RLIMIT_MSGQUEUE(value f)   { return Qlong(RLIMIT_MSGQUEUE); }
value fexl_RLIMIT_NICE(value f)       { return Qlong(RLIMIT_NICE); }
value fexl_RLIMIT_NOFILE(value f)     { return Qlong(RLIMIT_NOFILE); }
value fexl_RLIMIT_NPROC(value f)      { return Qlong(RLIMIT_NPROC); }
value fexl_RLIMIT_RSS(value f)        { return Qlong(RLIMIT_RSS); }
value fexl_RLIMIT_RTPRIO(value f)     { return Qlong(RLIMIT_RTPRIO); }
value fexl_RLIMIT_RTTIME(value f)     { return Qlong(RLIMIT_RTTIME); }
value fexl_RLIMIT_SIGPENDING(value f) { return Qlong(RLIMIT_SIGPENDING); }
value fexl_RLIMIT_STACK(value f)      { return Qlong(RLIMIT_STACK); }

/* (setrlimit resource soft hard next) = (next status) */
value fexl_setrlimit(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;

	value x = f->L->L->L->R;
	value y = f->L->L->R;
	value z = f->L->R;

	if (!arg(type_long,x)) return 0;
	if (!arg(type_long,y)) return 0;
	if (!arg(type_long,z)) return 0;

	long resource = get_long(x);
	struct rlimit rlim;
	rlim.rlim_cur = get_long(y);
	rlim.rlim_max = get_long(z);

	int status = setrlimit(resource, &rlim);
	return A(f->R,Qlong(status));
	}

/* (getrlimit resource next) = (next status soft hard) */
value fexl_getrlimit(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = f->L->R;
	if (!arg(type_long,x)) return 0;

	long resource = get_long(x);
	struct rlimit rlim;
	int status = getrlimit(resource, &rlim);

	return A(A(A(f->R,Qlong(status)),
		Qlong(rlim.rlim_cur)),
		Qlong(rlim.rlim_max));
	}

/* LATER getrusage */
