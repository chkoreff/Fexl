#include <value.h>

#include <basic.h>

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

/* (F x) = I */
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

/* (void x) = void */
value type_void(value f)
	{
	if (!f->L) return 0;
	return hold(Qvoid);
	}

/* (cons x y A B) = (B x y) */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	return A(A(hold(f->R),hold(f->L->L->L->R)),hold(f->L->L->R));
	}

/* (null A B) = A */
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

/* (O x) Evaluate x once, replacing the right side with the final value. */
value type_O(value f)
	{
	value x = arg(f->R);
	drop(f->R);
	f->T = type_I;
	f->R = hold(x);
	return x;
	}

/* (once x) Return a value which will evaluate x at most once. */
value type_once(value f)
	{
	if (!f->L) return 0;
	f->T = type_O;
	return 0;
	}

/* (later x) Return x without evaluating it. */
value type_later(value f)
	{
	if (!f->L) return 0;
	f->T = type_I;
	return 0;
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

static int op_is_list(value x)
	{
	return (x->T == type_null && !x->L)
		|| (x->T == type_cons && x->L && x->L->L);
	}

value type_is_good(value f) { return op_predicate(f,op_is_good); }
value type_is_bool(value f) { return op_predicate(f,op_is_bool); }
value type_is_list(value f) { return op_predicate(f,op_is_list); }

value boolean(int x)
	{
	return hold(x ? QT : QF);
	}

value QI;
value QT;
value QF;
value QY;
value Qvoid;
value Qcons;
value Qnull;
value Qeval;

void beg_basic(void)
	{
	QI = Q(type_I);
	QT = Q(type_T);
	QF = Q(type_F);
	QY = Q(type_Y);
	Qvoid = Q(type_void);
	Qcons = Q(type_cons);
	Qnull = Q(type_null);
	Qeval = Q(type_eval);
	}

void end_basic(void)
	{
	drop(QI);
	drop(QT);
	drop(QF);
	drop(QY);
	drop(Qvoid);
	drop(Qcons);
	drop(Qnull);
	drop(Qeval);
	}
