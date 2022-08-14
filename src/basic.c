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
	if (x->T == 0)
		return AV(hold(Qlist),hold(x));
	else
		return hold(x);
	}

/* ([x;y] a b) = (b x y) */
value type_list(value f)
	{
	if (!f->L->L || !f->L->L->L) return 0;
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

/* Return the next item from a list, advancing to the tail of the list. */
value iterate(value *p)
	{
	value x = *p;
	value item;
	value tail;

	if (x->T == 0)
		{
		item = x->L;
		tail = x->R;
		}
	else
		{
		x = arg(x);
		if (x->T == type_list && !x->L->L)
			{
			item = x->R->L;
			tail = x->R->R;
			}
		else if (x->T == type_cons && x->L && x->L->L && !x->L->L->L)
			{
			item = x->L->R;
			tail = x->R;
			}
		else
			{
			item = 0;
			tail = 0;
			}
		drop(x);
		}

	*p = tail;
	return item;
	}
