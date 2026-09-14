#include <value.h>

#include <basic.h>
#include <define.h>

value QI;
value QT;
value QF;
static value QY;
value Qvoid;
value Qnull;
value Qonce;

// (I x) = x
static value type_I(value f)
	{
	return hold(f->R);
	}

// (T x y) = x
value type_T(value f)
	{
	if (f->L->L == 0) return keep(f);
	return hold(f->L->R);
	}

// (F x y) = y
value type_F(value f)
	{
	if (f->L->L == 0) return keep(f);
	return hold(f->R);
	}

// (Y x) = (x (Y x))
static value type_Y(value f)
	{
	return A(hold(f->R),hold(f));
	}

// (void x) = void
value type_void(value f)
	{
	return hold(Qvoid);
	(void)f;
	}

// ((pair x y) h) = (h x y)
value type_pair(value f)
	{
	value x = hold(f->L->L);
	value y = hold(f->L->R);
	value h = hold(f->R);
	return A(A(h,x),y);
	}

value pair(value x, value y)
	{
	return V(type_pair,x,y);
	}

// ((list x y) h) = (pair x y)
value type_list(value f)
	{
	return pair(hold(f->L->L),hold(f->L->R));
	}

value cons(value x, value y)
	{
	return V(type_list,x,y);
	}

// (cons x y) = (list x y)
static value type_cons(value f)
	{
	if (f->L->L == 0) return keep(f);
	return cons(hold(f->L->R),hold(f->R));
	}

// (null x y) = x
value type_null(value f)
	{
	f->T = type_T;
	return hold(f);
	}

// (eval x f) = (f y), where y is the final value of x.
static value type_eval(value f)
	{
	if (f->L->L == 0) return keep(f);
	return A(hold(f->R),arg(f->L->R));
	}

// Evaluate x once, replacing the right side with the final value.
static value type_once(value f)
	{
	return hold(f->R = eval(f->R));
	}

// (yield x f) = (f x)  Used for returning an unevaluated function.
static value type_yield(value f)
	{
	if (f->L->L == 0) return keep(f);
	return A(hold(f->R),hold(f->L->R));
	}

value yield(value x)
	{
	return V(type_yield,hold(QI),x);
	}

// Return either no or (yes x).
value maybe(value x)
	{
	if (x == 0)
		return hold(QT);
	else
		return V(type_T,hold(QT),yield(hold(x)));
	}

value boolean(int x)
	{
	return hold(x ? QT : QF);
	}

static value type_is_defined(value f)
	{
	return boolean(f->R->T != type_void);
	}

static value type_is_undef(value f)
	{
	return boolean(f->R->T == type_void);
	}

value op_is_type(value f, type t)
	{
	value x = arg(f->R);
	f = boolean(x->T == t);
	drop(x);
	return f;
	}

static value type_is_void(value f)
	{
	return op_is_type(f,type_void);
	}

static value op_predicate(value f, int op(value))
	{
	value x = arg(f->R);
	f = boolean(op(x));
	drop(x);
	return f;
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
	return x->T == type_list || x->T == type_null;
	}

static value type_is_good(value f) { return op_predicate(f,is_good); }
static value type_is_bool(value f) { return op_predicate(f,is_bool); }
static value type_is_list(value f) { return op_predicate(f,is_list); }

/*
(:: a b) x
Return (a x) if that is defined, otherwise return (b x).

\::=
	(
	\\a \\a=(\= a)
	\\b \\b=(\= b)
	\x
	\v=(a x)
	is_defined v v (b x)
	)
*/
static value type_chain(value f)
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	{
	value x = arg(f->R);
	value a = (f->L->L->R = eval(f->L->L->R));
	value v = eval(A(hold(a),x));

	if (v->T != type_void)
		return v;

	drop(v);
	{
	value b = (f->L->R = eval(f->L->R));
	return A(hold(b),hold(x));
	}
	}
	}

// Create a nullary function, supplying a dummy argument.
value Q0(type T)
	{
	return A(Q(T),hold(QI));
	}

void define_basic(void)
	{
	define("I",hold(QI));
	define("T",hold(QT));
	define("F",hold(QF));
	define("@",hold(QY));
	define("void",hold(Qvoid));
	define("null",hold(Qnull));
	define("once",hold(Qonce));
	define("cons",Q(type_cons));
	define("eval",Q(type_eval));
	define("yield",Q(type_yield));
	define("is_defined",Q(type_is_defined));
	define("is_undef",Q(type_is_undef));
	define("is_void",Q(type_is_void));
	define("is_good",Q(type_is_good));
	define("is_bool",Q(type_is_bool));
	define("is_list",Q(type_is_list));
	define("::",Q(type_chain));
	}

void beg_basic(void)
	{
	QI = Q(type_I);
	QT = Q(type_T);
	QF = Q(type_F);
	QY = Q(type_Y);
	Qvoid = Q(type_void);
	Qnull = Q(type_null);
	Qonce = Q(type_once);
	}

void end_basic(void)
	{
	drop(QI);
	drop(QT);
	drop(QF);
	drop(QY);
	drop(Qvoid);
	drop(Qnull);
	drop(Qonce);
	}
