#include <num.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <standard.h>
#include <type_num.h>
#include <type_sym.h>
#include <type_tuple.h>

/* (tuple data handler) = (data handler) */
value type_tuple(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return A(hold(f->L->R),hold(f->R));
	}

value type_is_tuple(value f)
	{
	return op_is_type(f,type_tuple);
	}

/* (arity type) Return the number of elements in the tuple. */
value type_arity(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_tuple)
		{
		value pattern = x->R->L->R;
		unsigned long arity = 0;
		while (pattern != QT)
			{
			arity++;
			pattern = pattern->L;
			}
		f = Qnum(num_new_ulong(arity));
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

static value make_tuple(value pattern, value exp)
	{
	return A(hold(Qtuple),Qsubst(pattern,exp));
	}

/* (split_tuple xs A B)
Return A if tuple xs is empty, otherwise return (B left item), where left is
the left part of the tuple, and item is the last item in the tuple.
*/
value type_split_tuple(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	if (x->T == type_tuple)
		{
		value pattern = x->R->L->R;
		if (pattern == QT)
			f = hold(f->L->R);
		else
			{
			value exp = x->R->R;
			value left = make_tuple(hold(pattern->L),hold(exp->L));
			value item = hold(exp->R);
			f = A(A(hold(f->R),left),item);
			}
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (join_tuple xs x) Form a new tuple by combining tuple xs on the left with
item x on the right. */
value type_join_tuple(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_tuple)
		{
		value pattern = x->R->L->R;
		value exp = x->R->R;
		pattern = A(hold(pattern),hold(QF));
		exp = A(hold(exp),hold(f->R));
		f = make_tuple(pattern,exp);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}
