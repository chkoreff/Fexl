#include <value.h>
#include <basic.h>
#include <type_var.h>

/* A var is a mutable variable where you can put and get values.  This can help
with things like caching, simulating a dynamic entity such as a file system or
human user, redefining print to capture output in a memory buffer, etc. */

value type_var(value f)
	{
	return type_void(f);
	}

/* var_new returns a new variable with a void value. */
value type_var_new(value f)
	{
	(void)f;
	return D(type_var,Q(type_void),(type)drop);
	}

/* (var_get var) returns val, where val is the current value of var. */
value type_var_get(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_var)
		return hold(data(x));
	reduce_void(f);
	return 0;
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
		value v = hold(arg(f->R));
		drop(x->R->R);
		x->R->R = v;
		return Q(type_I);
		}
	reduce_void(f);
	return 0;
	}
	}
