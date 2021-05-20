#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_num.h>
#include <type_tuple.h>

value Qtuple;

static value subst_tuple(value args, value handler)
	{
	if (args->T == type_I)
		return hold(handler);
	else
		return A(subst_tuple(args->R,handler),hold(args->L));
	}

value type_tuple(value f)
	{
	if (!f->L->L) return 0;
	return subst_tuple(f->L->R,f->R);
	}

value type_is_tuple(value f)
	{
	return op_is_type(f,type_tuple);
	}

/* (arity type) Return the number of elements in the tuple. */
value type_arity(value f) /* LATER 20210519 deprecate */
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_tuple)
		{
		unsigned long arity = 0;
		value args = x->R;
		while (args->T != type_I)
			{
			arity++;
			args = args->R;
			}
		f = Qnum((double)arity);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (split_tuple x A B)
Return A if tuple x is empty, otherwise return (B left item), where left is
the left part of the tuple, and item is the last item in the tuple.
*/
value type_split_tuple(value f) /* LATER 20210519 deprecate */
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	if (x->T == type_tuple)
		{
		value args = x->R;
		if (args->T == type_I)
			f = hold(f->L->R);
		else
			{
			value left = AV(hold(Qtuple),hold(args->R));
			value item = hold(args->L);
			f = A(A(hold(f->R),left),item);
			}
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (join_tuple x y) Form a new tuple by combining tuple x on the left with
item y on the right. */
value type_join_tuple(value f) /* LATER 20210519 deprecate */
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_tuple)
		f = AV(hold(Qtuple),A(hold(f->R),hold(x->R)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value type_tuple_to_list(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_tuple)
		{
		value args = x->R;
		f = hold(Qnull);
		while (args->T == type_A)
			{
			f = AV(AV(hold(Qcons),hold(args->L)),f);
			args = args->R;
			}
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value type_list_to_tuple(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	value args = hold(QI);
	while (1)
		{
		if (is_cons(x))
			{
			value head = hold(x->L->R);
			value tail = arg(x->R);
			args = A(head,args);
			drop(x);
			x = tail;
			}
		else if (is_null(x))
			{
			f = AV(hold(Qtuple),args);
			break;
			}
		else
			{
			drop(args);
			f = hold(Qvoid);
			break;
			}
		}
	drop(x);
	return f;
	}
	}
