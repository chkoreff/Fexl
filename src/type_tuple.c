#include <value.h>

#include <basic.h>
#include <type_tuple.h>

value Qtuple;

value type_tuple(value f)
	{
	value args = hold(f->L->R);
	value exp = hold(f->R);

	while (1)
		{
		args = eval(args);
		if (args->T == type_list)
			{
			exp = A(exp,hold(args->L));
			args = tail(args);
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

value type_is_tuple(value f)
	{
	value x = arg(f->R);
	f = boolean(x->T == type_tuple || x->T == type_pair);
	drop(x);
	return f;
	}

value type_tuple_to_list(value f)
	{
	value x = arg(f->R);
	if (x->T == type_tuple)
		f = hold(x->R);
	else if (x->T == type_pair)
		f = cons(hold(x->L),cons(hold(x->R),hold(Qnull)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

value type_list_to_tuple(value f)
	{
	return V(type_tuple,hold(Qtuple),hold(f->R));
	}
