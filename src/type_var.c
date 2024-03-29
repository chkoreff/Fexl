#include <value.h>

#include <basic.h>
#include <type_var.h>

// A var is a mutable variable where you can put and get values.  This can help
// with things like caching, simulating a dynamic entity such as a file system
// or human user, redefining print to capture output in a memory buffer, etc.

value type_var(value f)
	{
	return type_void(f);
	}

static void clear_var(value f)
	{
	drop(f->R);
	}

// (var_new) Return a new variable with a void value.
value type_var_new(value f)
	{
	static struct value clear = {{.N=0}, {.clear=clear_var}};
	return V(type_var,&clear,hold(Qvoid));
	(void)f;
	}

// (var_get var) Return the current value of var.
value type_var_get(value f)
	{
	value x = arg(f->R);
	if (x->T == type_var)
		f = hold(x->R);
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

// (var_getf var) Yield the current value of var.
value type_var_getf(value f)
	{
	value x = arg(f->R);
	if (x->T == type_var)
		f = yield(hold(x->R));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value op_put(value f, value op(value))
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	if (x->T == type_var)
		{
		value v = op(f->R);
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

// (var_put var x) Put the value of x into var.
value type_var_put(value f)
	{
	return op_put(f,arg);
	}

// (var_putf var x) Put x into var.
value type_var_putf(value f)
	{
	return op_put(f,hold);
	}

value type_is_var(value f)
	{
	return op_is_type(f,type_var);
	}
