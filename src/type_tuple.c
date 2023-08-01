#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_tuple.h>

value Qtuple;

value type_tuple(value fun, value f)
	{
	expand(fun);
	{
	value exp = hold(f->R);
	value args = fun->R;
	while (args->T == type_list)
		{
		exp = A(exp,hold(args->L));
		args = args->R;
		}
	return exp;
	}
	}

value type_is_tuple(value fun, value f)
	{
	return op_is_type(fun,f,type_tuple);
	}

value type_tuple_to_list(value fun, value f)
	{
	value x = arg(f->R);
	if (x->T == type_tuple)
		f = hold(x->R);
	else
		f = hold(Qvoid);
	drop(x);
	(void)fun;
	return f;
	}

value type_list_to_tuple(value fun, value f)
	{
	(void)fun;
	return V(type_tuple,hold(Qtuple),hold(f->R));
	}

value pair(value x, value y)
	{
	return V(type_tuple,hold(Qtuple),
		V(type_list,x,
		V(type_list,y,hold(Qnull))));
	}

void beg_tuple(void)
	{
	Qtuple = Q(type_tuple);
	}

void end_tuple(void)
	{
	drop(Qtuple);
	}
