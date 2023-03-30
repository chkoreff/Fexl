#include <value.h>

#include <basic.h>
#include <type_var.h>

/* A var is a mutable variable where you can put and get values.  This can help
with things like caching, simulating a dynamic entity such as a file system or
human user, redefining print to capture output in a memory buffer, etc. */

value type_var(value f)
	{
	return type_atom(f);
	}

static void clear_var(value f)
	{
	drop(f->R); // LATER 20230329 This makes drop recursive.
	}

/* var_new returns a new variable with a void value. */
value type_var_new(value f)
	{
	static struct value atom = {0, {.clear=clear_var}};
	(void)f;
	return V(type_var,&atom,hold(Qvoid));
	}

/* (var_get var) returns val, where val is the current value of var. */
value type_var_get(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_var)
		f = hold(x->R);
	else
		f = hold(Qvoid);
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
		drop(x->R);
		x->R = v;
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value type_is_var(value f)
	{
	return op_is_type(f,type_var);
	}
