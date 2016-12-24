#include <value.h>

#include <basic.h>
#include <type_var.h>

/* A var is a mutable variable where you can put and get values.  This can help
with things like caching, simulating a dynamic entity such as a file system or
human user, redefining print to capture output in a memory buffer, etc. */

value type_var(value f)
	{
	if (f->N == 0)
		{
		drop(get_var(f));
		return 0;
		}
	return type_void(f);
	}

value get_var(value x)
	{
	return data(x);
	}

/* var_new returns a new variable with a void value. */
value type_var_new(value f)
	{
	(void)f;
	return D(type_var,hold(&Qvoid));
	}

/* (var_get var) returns val, where val is the current value of var. */
value type_var_get(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_var)
		f = hold(get_var(x));
	else
		f = reduce_void(f);
	drop(x);
	return f;
	}
	}

/* (var_put var val) replaces the value of the variable. */
value type_var_put(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_var)
		{
		value v = arg(f->R);
		drop(x->R->R);
		x->R->R = v;
		f = hold(&QI);
		}
	else
		f = reduce_void(f);
	drop(x);
	return f;
	}
	}
