#include <value.h>

#include <basic.h>

value QI;
value QT;
value QF;
value QY;
value Qvoid;
value Qlist;
value Qcons;
value Qnull;
value Qeval;
value Qonce;
value Qyield;

/* (I x) = x */
value type_I(value f)
	{
	if (!f->L) return 0;
	return hold(f->R);
	}

/* (T x y) = x */
value type_T(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return hold(f->L->R);
	}

/* (F x y) = y */
value type_F(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return hold(f->R);
	}

/* (Y x) = (x (Y x)) */
value type_Y(value f)
	{
	if (!f->L) return 0;
	return AV(arg(f->R),hold(f));
	}

value type_atom(value f)
	{
	if (!f->L->N) return 0;
	return hold(Qvoid);
	}

/* (void x) = void */
value type_void(value f)
	{
	if (!f->L) return 0;
	return hold(Qvoid);
	}

/* Wrap list function around data if necessary. */
value wrap(value x)
	{
	value y = hold(x);
	if (y->T == 0)
		return AV(hold(Qlist),y);
	else
		return y;
	}

/* ([x;y] a b) = (b x y) */
value type_list(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value z = f->L->L->R;
	value x = hold(z->L);
	value y = wrap(z->R);

	return AV(eval(AV(arg(f->R),x)),y);
	}
	}

/* (cons x y a b) = (b x y) */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	{
	value z = f->L->L;
	value x = hold(z->L->R);
	value y = hold(z->R);

	return AV(eval(AV(arg(f->R),x)),y);
	}
	}

/* (null a b) = a */
value type_null(value f)
	{
	return type_T(f);
	}

/* (eval x f) = (f y), where y is the final value of x. */
value type_eval(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value y = arg(f->L->R);
	return AV(arg(f->R),y);
	}
	}

/* Evaluate x once, replacing the right side with the final value. */
value type_once(value f)
	{
	value x = arg(f->R);
	drop(f->R);
	f->T = type_I;
	f->R = hold(x);
	return x;
	}

/* (yield x f) = (f x)  Used for returning an unevaluated function. */
value type_yield(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return AV(arg(f->R),hold(f->L->R));
	}

value boolean(int x)
	{
	return hold(x ? QT : QF);
	}

value type_is_defined(value f)
	{
	if (!f->L) return 0;
	return boolean(f->R->T != type_void);
	}

value type_is_undef(value f)
	{
	if (!f->L) return 0;
	return boolean(f->R->T == type_void);
	}

value op_is_type(value f, type t)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	f = boolean(x->T == t);
	drop(x);
	return f;
	}
	}

value type_is_void(value f)
	{
	return op_is_type(f,type_void);
	}

static value op_predicate(value f, int op(value x))
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	f = boolean(op(x));
	drop(x);
	return f;
	}
	}

static int is_good(value x)
	{
	return (x->T != type_void);
	}

static int is_bool(value x)
	{
	return (x->T == type_T || x->T == type_F) && !x->L;
	}

static int is_list(value x)
	{
	return
		(x->T == type_list && !x->L->L) ||
		(x->T == type_cons && x->L && x->L->L && !x->L->L->L) ||
		(x->T == type_null && !x->L);
	}

value type_is_good(value f) { return op_predicate(f,is_good); }
value type_is_bool(value f) { return op_predicate(f,is_bool); }
value type_is_list(value f) { return op_predicate(f,is_list); }

/* Expand list data inline. */
void expand(value p)
	{
	while (1)
		{
		value x = p->R;
		if (x->T == 0)
			{
			p = x;
			continue;
			}

		x = (p->R = eval(x));

		if (x->T == type_cons && x->L && x->L->L && !x->L->L->L)
			{
			/* Change x inline from type_cons to type_list. */
			value data = V(0,hold(x->L->R),arg(x->R));
			drop(x->L);
			drop(x->R);
			x->T = type_list;
			x->L = hold(Qlist);
			x->R = hold(data);
			drop(x);
			p->R = data;
			}
		else if (x->T == type_list && !x->L->L)
			{
			value data = hold(x->R);
			drop(x);
			p->R = data;
			}
		else if (x->T == type_null && !x->L)
			break;
		else
			{
			/* Replace non-list tail with null. */
			drop(x);
			p->R = hold(Qnull);
			break;
			}
		}
	}
