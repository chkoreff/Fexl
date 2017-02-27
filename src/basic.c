#include <value.h>

#include <basic.h>

/* Boolean types */
static value op_boolean(value f, int flag)
	{
	if (!f->L || !f->L->L) return 0;
	return reduce(f,hold(flag ? f->L->R : f->R));
	}

value type_T(value f) { return op_boolean(f,1); } /* (T x y) = x */
value type_F(value f) { return op_boolean(f,0); } /* (F x y) = y */

/* (Y x) = (x (Y x)) */
value type_Y(value f)
	{
	if (!f->L) return 0;
	return reduce_A(f,hold(f->R),V(type_Y,hold(f->L),hold(f->R)));
	}

/* (eval x) yields the final value of x. */
value type_eval(value f)
	{
	if (!f->L) return 0;
	{
	value x = argp(&f->R);
	if (x != f->R) return V(type_yield,hold(&Qyield),x);
	f->T = type_yield;
	drop(x);
	return f;
	}
	}

/* (once x) = x, but x is evaluated only once. */
value type_once(value f)
	{
	if (!f->L) return 0;
	return reduce(f,arg(f->R));
	}

/* (void x) = void */
value type_void(value f)
	{
	if (!f->L) return 0;
	return reduce_void(f);
	}

/* (yield x p) = (p x) Used to return a function without evaluating it. */
value type_yield(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return reduce_A(f,hold(f->R),hold(f->L->R));
	}

/* (cons x y A B) = (B x y) */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	return reduce_A(f,A(hold(f->R),hold(f->L->L->L->R)),hold(f->L->L->R));
	}

/* (null A B) = A */
value type_null(value f)
	{
	return type_T(f);
	}

value op_is_type(value f, type t)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	f = reduce_boolean(f,(x->T == t));
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
	f = reduce_boolean(f,op(x));
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

struct value QT = { 1, type_T };
struct value QF = { 1, type_F };
struct value Qeval = { 1, type_eval };
struct value Qvoid = { 1, type_void };
struct value Qyield = { 1, type_yield };
struct value Qcons = { 1, type_cons };
struct value Qnull = { 1, type_null };

value reduce_void(value f)
	{
	return reduce_Q(f,type_void);
	}

value reduce_boolean(value f, int x)
	{
	return reduce_Q(f,(x ? type_T : type_F));
	}

value reduce_yield(value f, value x)
	{
	drop(f->L);
	drop(f->R);
	f->T = type_yield;
	f->L = hold(&QI);
	f->R = x;
	return 0;
	}
