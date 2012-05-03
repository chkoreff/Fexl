#include <string.h> /* strlen */
#include <stdlib.h> /* exit */
#include <sys/types.h> /* pid_t */
#include <sys/wait.h> /* wait */
#include <unistd.h> /* fork */
#include "base.h"
#include "memory.h"
#include "value.h"
#include "eval.h"
#include "long.h"
#include "run.h"
#include "string.h"

static value op_put_long(value f, long *p)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	if (!arg(type_long,x)) return f;

	*p = get_long(x);
	return f->R;
	}

static value op_get_long(value f, long *p)
	{
	return A(f->R,Qlong(*p));
	}

value fexl_put_max_steps(value f)
	{
	return op_put_long(f,&max_steps);
	}

value fexl_put_max_bytes(value f)
	{
	return op_put_long(f,&max_bytes);
	}

value fexl_get_max_steps(value f)
	{
	return op_get_long(f,&max_steps);
	}

value fexl_get_max_bytes(value f)
	{
	return op_get_long(f,&max_bytes);
	}

/* (argc next) = next val */
value fexl_argc(value f)
	{
	return A(f->R,Qlong(main_argc));
	}

/* (argv i next) = next val */
value fexl_argv(value f)
	{
	if (!f->L->L) return f;
	value x = f->L->R;
	if (!arg(type_long,x)) return f;

	long i = get_long(x);
	return A(f->R,Qstring(i < main_argc ? main_argv[i] : ""));
	}

/* (envc next) = next val */
value fexl_envc(value f)
	{
	return A(f->R,Qlong(main_envc()));
	}

/* (envp i next) = next val */
value fexl_envp(value f)
	{
	if (!f->L->L) return f;
	value x = f->L->R;
	if (!arg(type_long,x)) return f;

	long i = get_long(x);
	return A(f->R,Qstring(i < main_envc() ? main_envp[i] : ""));
	}

/* (exit status) exits immediately with the given status. */
value fexl_exit(value f)
	{
	if (!f->L->L) return f;
	value x = f->L->R;
	if (!arg(type_long,x)) return f;

	long status = get_long(x);
	exit(status);
	return f->R; /* never reached */
	}

/* (base_path next) = (next path) where path is the base path of the current
Fexl executable. */
value fexl_base_path(value f)
	{
	return A(f->R,Qstring(base));
	}

/*
LATER wrap a function around fork which reads stdout and stderr of the child.
*/

/* (fork next) = (next pid), where pid is the result of calling fork(2). */
value fexl_fork(value f)
	{
	return A(f->R,Qlong(fork()));
	}

/* (wait next) = (next pid status) */
value fexl_wait(value f)
	{
	int status;
	pid_t pid = wait(&status);
	return A(A(f->R,Qlong(pid)),Qlong(status));
	}
