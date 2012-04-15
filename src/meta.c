#include <string.h> /* strlen */
#include "memory.h"
#include "value.h"
#include "eval.h"
#include "long.h"
#include "run.h"
#include "string.h"

value fexl_put_max_steps(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	if (!arg(type_long,x)) return f;

	max_steps = get_long(x);
	return f->R;
	}

value fexl_put_max_bytes(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	if (!arg(type_long,x)) return f;

	max_bytes = get_long(x);
	return f->R;
	}

value fexl_get_max_steps(value f)
	{
	return A(f->R,Qlong(max_steps));
	}

value fexl_get_max_bytes(value f)
	{
	return A(f->R,Qlong(max_bytes));
	}

static value op_access_string_array(value f, long len, char *array[])
	{
	if (!f->L->L) return f;
	value x = f->L->R;
	if (!arg(type_long,x)) return f;

	long i = get_long(x);
	char *str = i < len ? array[i] : "";
	value val = Qchars(str,strlen(str));
	val->R->T = 0;  /* Don't clear static string. */
	return A(f->R,val);
	}

/* (argc next) = next val */
value fexl_argc(value f)
	{
	return A(f->R,Qlong(main_argc));
	}

/* (argv i next) = next val */
value fexl_argv(value f)
	{
	return op_access_string_array(f, main_argc, main_argv);
	}

/* (envc next) = next val */
value fexl_envc(value f)
	{
	return A(f->R,Qlong(main_envc()));
	}

/* (envp i next) = next val */
value fexl_envp(value f)
	{
	return op_access_string_array(f, main_envc(), main_envp);
	}

/*
LATER a combinator to fork another Fexl and read stdout and stderr so you can
run other functions safely without affecting the parent, and gather stdout and
stderr too.
*/
