#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_num.h>
#include <type_tuple.h>

value Qtuple;

/* Apply the expression to all the arguments. */
static value apply(value exp, value args)
	{
	if (args->T == type_A)
		return AV(eval(apply(exp,args->R)),hold(args->L));
	else
		return hold(exp);
	}

value type_tuple(value f)
	{
	if (!f->L->L) return 0;
	return apply(f->R,f->L->R);
	}

value type_is_tuple(value f)
	{
	return op_is_type(f,type_tuple);
	}

/* Convert tuple to list. */
static value tuple_to_list(value curr)
	{
	value exp = hold(Qnull);
	while (curr->T == type_A)
		{
		exp = AV(AV(hold(Qcons),hold(curr->L)),exp);
		curr = curr->R;
		}
	return exp;
	}

value type_tuple_to_list(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_tuple)
		f = tuple_to_list(x->R);
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* Convert list to tuple. */
static value list_to_tuple(value curr)
	{
	value args = hold(QI);
	while (curr->T == type_cons)
		{
		args = A(hold(curr->L->R),args);
		curr = curr->R;
		}
	return args;
	}

value type_list_to_tuple(value f)
	{
	if (!f->L) return 0;
	{
	f->R = eval(f->R);
	if (is_list(f->R))
		{
		expand(f);
		f = AV(hold(Qtuple),list_to_tuple(f->R));
		}
	else
		f = hold(Qvoid);
	return f;
	}
	}
