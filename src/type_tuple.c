#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_tuple.h>

value Qtuple;

value type_tuple(value fun, value f)
	{
	value args = hold(fun->R);
	value exp = hold(f->R);

	while (1)
		{
		args = eval(args);
		if (args->T == type_list)
			{
			value head = hold(args->L);
			value tail = hold(args->R);
			exp = A(exp,head);
			drop(args);
			args = tail;
			}
		else if (args->T == type_null)
			{
			drop(args);
			return exp;
			}
		else
			{
			drop(exp);
			drop(args);
			return hold(Qvoid);
			}
		}
	}

value type_is_tuple(value fun, value f)
	{
	value x = arg(f->R);
	f = boolean(x->T == type_tuple || x->T == type_pair);
	drop(x);
	(void)fun;
	return f;
	}

value type_tuple_to_list(value fun, value f)
	{
	value x = arg(f->R);
	if (x->T == type_tuple)
		f = hold(x->R);
	else if (x->T == type_pair)
		f = V(type_list,hold(x->L),V(type_list,hold(x->R),hold(Qnull)));
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

void beg_tuple(void)
	{
	Qtuple = Q(type_tuple);
	}

void end_tuple(void)
	{
	drop(Qtuple);
	}
