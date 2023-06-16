#include <value.h>

#include <basic.h>
#include <context.h>

#include <type_var.h>

// Variable

static void clear_var(value f)
	{
	drop(f->R);
	}

value type_var(value fun, value arg)
	{
	return type_void(fun,arg);
	}

// Create variable
value type_var_new(value fun, value arg)
	{
	drop(fun);
	drop(arg);
	{
	static struct value clear = {{.N=0}, {.clear=clear_var}};
	return V(type_var,&clear,hold(Qvoid));
	}
	}

// Put value in variable
value type_var_put(value fun, value arg)
	{
	if (fun->L == 0)
		return need(fun,arg,type_var);
	else
		{
		value v = fun->R;
		arg = eval(arg);
		drop(v->R);
		v->R = arg;
		drop(fun);
		return hold(QI);
		}
	}

// Get value from variable
value type_var_get(value fun, value arg)
	{
	arg = eval(arg);
	if (arg->T == type_var)
		{
		value g = hold(arg->R);
		drop(fun);
		drop(arg);
		return g;
		}
	else
		return type_void(fun,arg);
	}

void use_var(void)
	{
	define("var_new", A(Q(type_var_new),hold(QI)));
	define("var_put", Q(type_var_put));
	define("var_get", Q(type_var_get));
	}
