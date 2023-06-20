#include <value.h>

#include <basic.h>

value QI;
value QT;
value QF;
value Qvoid;
value Qnull;

value keep(value fun, value arg)
	{
	return V(fun->T,fun,arg);
	}

value need(value fun, value arg, type T)
	{
	arg = eval(arg);
	if (arg->T == T)
		return keep(fun,arg);
	else
		return type_void(fun,arg);
	}

// (I x) = x
value type_I(value fun, value arg)
	{
	drop(fun);
	return arg;
	}

// (T x y) = x
value type_T(value fun, value arg)
	{
	if (fun->L == 0)
		return keep(fun,arg);
	else
		{
		value x = hold(fun->R);
		drop(fun);
		drop(arg);
		return x;
		}
	}

// (F x y) = y
value type_F(value fun, value arg)
	{
	drop(fun);
	drop(arg);
	return hold(QI);
	}

// (Y x) = (x (Y x))  # fixpoint operator
value type_Y(value fun, value arg)
	{
	return A(arg,A(fun,hold(arg)));
	}

// (void x) = void
value type_void(value fun, value arg)
	{
	drop(fun);
	drop(arg);
	return hold(Qvoid);
	}

// Use pattern p to make a copy of expression e with argument x substituted in
// the places designated by the pattern.
value subst(value p, value e, value x)
	{
	if (p == QF) return hold(e);
	if (p == QT) return hold(x);
	{
	value L = subst(p->L,e->L,x);
	value R = subst(p->R,e->R,x);
	return V(e->T,L,R);
	}
	}

// Direct substitution
value type_D(value fun, value arg)
	{
	value g = subst(fun->L,fun->R,arg);
	drop(fun);
	drop(arg);
	return g;
	}

// Eager substitution
value type_E(value fun, value arg)
	{
	return type_D(fun,eval(arg));
	}

// (single x h) = (h x)
value type_single(value fun, value arg)
	{
	value x = hold(fun->R);
	drop(fun);
	return A(arg,x);
	}

value single(value x)
	{
	return V(type_single,hold(QI),x);
	}

value maybe(value x)
	{
	if (x == 0)
		return hold(QT);
	else
		return V(type_T,hold(QT),single(x));
	}

value type_yield(value fun, value arg)
	{
	drop(fun);
	return single(arg);
	}

// ((pair x y) h) = (h x y)
value type_pair(value fun, value arg)
	{
	value x = hold(fun->L);
	value y = hold(fun->R);
	value h = arg;
	drop(fun);
	return A(A(h,x),y);
	}

value Qpair(value x, value y)
	{
	return V(type_pair,x,y);
	}

// (null x y) = x
value type_null(value fun, value arg)
	{
	return type_T(fun,arg);
	}

// ((list x y) h) = (pair x y)
value type_list(value fun, value arg)
	{
	value x = hold(fun->L);
	value y = hold(fun->R);
	drop(fun);
	drop(arg);
	return Qpair(x,y);
	}

// (cons x y) = (list x y)
value type_cons(value fun, value arg)
	{
	if (fun->L == 0)
		return keep(fun,arg);
	else
		{
		value x = hold(fun->R);
		value y = arg;
		drop(fun);
		return V(type_list,x,y);
		}
	}

// Tuples

value type_tuple(value fun, value arg)
	{
	value list = hold(fun->R);
	while (1)
		{
		list = eval(list);
		if (list->T == type_null)
			{
			drop(list);
			drop(fun);
			return arg;
			}
		else if (list->T == type_list)
			{
			value item = hold(list->L);
			value tail = hold(list->R);
			drop(list);
			arg = A(arg,item);
			list = tail;
			}
		else
			{
			drop(fun);
			return A(arg,list); // improper list tail
			}
		}
	}

// Convert list to tuple, e.g. [1 2 3] to {1 2 3}.
value type_list_to_tuple(value fun, value arg)
	{
	drop(fun);
	return V(type_tuple,hold(QI),arg);
	}

// Convert to tuple to list, e.g. {1 2 3} to [1 2 3].
value type_tuple_to_list(value fun, value arg)
	{
	arg = eval(arg);
	if (arg->T == type_tuple)
		{
		value list = hold(arg->R);
		drop(fun);
		drop(arg);
		return list;
		}
	else
		return type_void(fun,arg);
	}

void beg_basic(void)
	{
	QI = Q(type_I);
	QT = Q(type_T);
	QF = Q(type_F);
	Qvoid = Q(type_void);
	Qnull = Q(type_null);
	}

void end_basic(void)
	{
	drop(QI);
	drop(QT);
	drop(QF);
	drop(Qvoid);
	drop(Qnull);
	}
