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

/* (var_new) returns {var}, where var is a new variable with a void value. */
value type_var_new(value f)
	{
	(void)f;
	return single(D(type_var,Q(type_void),(type)drop));
	}

/* (var_get var) returns {val}, where val is the current value of x. */
value type_var_get(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_var)
		f = single(hold(data(x)));
	else
		replace_void(f);
	drop(x);
	return f;
	}
	}

/* (var_put var val) replaces the value of the variable. */
value type_var_put(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = eval(hold(f->L->R));
	value y = hold(f->R);
	if (x->T == type_var)
		{
		drop(x->R->R);
		x->R->R = hold(y);
		f = Q(type_I);
		}
	else
		replace_void(f);
	drop(x);
	drop(y);
	return f;
	}
	}
