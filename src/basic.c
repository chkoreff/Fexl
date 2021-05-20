#include <value.h>

#include <basic.h>

value QI;
value QT;
value QF;
value QY;
value Qvoid;
value Qcons;
value Qnull;
value Qeval;
value Qonce;
value Qcatch;
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
	return A(hold(f->R),hold(f));
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

/* (cons x y a b) = (b x y) */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	return A(A(hold(f->R),hold(f->L->L->L->R)),hold(f->L->L->R));
	}

/* (null a b) = a */
value type_null(value f)
	{
	return type_T(f);
	}

/* (eval x next) = (next y), where y is the final value of x. */
value type_eval(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	return A(hold(f->R),x);
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

/* Catch arguments but do nothing.  Used internally for symbol resolution. */
value type_catch(value f)
	{
	(void)f;
	return 0;
	}

/* (yield x f) = (f x)  Used for returning an unevaluated function. */
value type_yield(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return A(hold(f->R),hold(f->L->R));
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

static int op_is_good(value x)
	{
	return (x->T != type_void);
	}

static int op_is_bool(value x)
	{
	return (x->T == type_T || x->T == type_F) && !x->L;
	}

int is_null(value x)
	{
	return (x->T == type_null && !x->L);
	}

int is_cons(value x)
	{
	return (x->T == type_cons && x->L && x->L->L && !x->L->L->L);
	}

static int op_is_list(value x)
	{
	return is_cons(x) || is_null(x);
	}

value type_is_good(value f) { return op_predicate(f,op_is_good); }
value type_is_bool(value f) { return op_predicate(f,op_is_bool); }
value type_is_list(value f) { return op_predicate(f,op_is_list); }
