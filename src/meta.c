#include <string.h> /* strlen */
#include <stdlib.h> /* exit */
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

value fexl_put_argi(value f)
	{
	return op_put_long(f,&argi);
	}

value fexl_get_max_steps(value f)
	{
	return op_get_long(f,&max_steps);
	}

value fexl_get_max_bytes(value f)
	{
	return op_get_long(f,&max_bytes);
	}

value fexl_get_argi(value f)
	{
	return op_get_long(f,&argi);
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

/*
LATER a combinator to fork another Fexl and read stdout and stderr so you can
run other functions safely without affecting the parent, and gather stdout and
stderr too.
*/
