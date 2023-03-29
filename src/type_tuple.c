#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_tuple.h>

value Qtuple;

value type_tuple(value f)
	{
	if (!f->L->L)
		{
		expand(f);
		return 0;
		}
	{
	value exp = hold(f->R);
	value args = f->L->R;
	while (args->T == 0)
		{
		exp = AV(eval(exp),hold(args->L));
		args = args->R;
		}
	return exp;
	}
	}

value type_is_tuple(value f)
	{
	return op_is_type(f,type_tuple);
	}

value type_tuple_to_list(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_tuple)
		f = wrap(x->R);
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value type_list_to_tuple(value f)
	{
	if (!f->L) return 0;
	return AV(hold(Qtuple),hold(f->R));
	}

value pair(value x, value y)
	{
	return AV(hold(Qtuple),V(0,x,V(0,y,hold(Qnull))));
	}

void beg_tuple(void)
	{
	Qtuple = Q(type_tuple);
	}

void end_tuple(void)
	{
	drop(Qtuple);
	}
