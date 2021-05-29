#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_num.h>
#include <type_tuple.h>

value Qtuple;

value type_tuple(value f)
	{
	if (!f->L->L) return 0;
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
	expand(f);
	return AV(hold(Qtuple),hold(f->R));
	}
