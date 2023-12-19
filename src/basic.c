#include <value.h>

#include <basic.h>

value QI;
value QT;
value QF;
value QY;
value Qvoid;
value Qnull;
value Qonce;
value Qyield;

// (I x) = x
value type_I(value fun, value f)
	{
	(void)fun;
	(void)f;
	return hold(f->R);
	}

// (T x y) = x
value type_T(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	return hold(fun->R);
	}

// (F x y) = y
value type_F(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	return hold(f->R);
	}

// (Y x) = (x (Y x))
value type_Y(value fun, value f)
	{
	return A(hold(f->R),A(hold(fun),hold(f->R)));
	}

// (void x) = void
value type_void(value fun, value f)
	{
	(void)fun;
	(void)f;
	return hold(Qvoid);
	}

// ((pair x y) h) = (h x y)
value type_pair(value fun, value f)
	{
	value x = hold(fun->L);
	value y = hold(fun->R);
	value h = hold(f->R);
	return A(A(h,x),y);
	}

// ((list x y) h) = (pair x y)
value type_list(value fun, value f)
	{
	value x = hold(fun->L);
	value y = hold(fun->R);
	(void)f;
	return V(type_pair,x,y);
	}

// (cons x y) = (list x y)
value type_cons(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value x = hold(fun->R);
	value y = hold(f->R);
	return V(type_list,x,y);
	}
	}

// (null x y) = x
value type_null(value fun, value f)
	{
	return V(type_T,hold(fun),hold(f->R));
	}

// (eval x f) = (f y), where y is the final value of x.
value type_eval(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	return A(hold(f->R),arg(fun->R));
	}

// Evaluate x once, replacing the right side with the final value.
value type_once(value fun, value f)
	{
	(void)fun;
	return (f->R = hold(eval(f->R)));
	}

// (yield x f) = (f x)  Used for returning an unevaluated function.
value type_yield(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	return A(hold(f->R),hold(fun->R));
	}

value yield(value x)
	{
	return V(type_yield,hold(Qyield),x);
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

value pair(value x, value y)
	{
	return V(type_pair,x,y);
	}

value type_is_defined(value fun, value f)
	{
	(void)fun;
	return boolean(f->R->T != type_void);
	}

value type_is_undef(value fun, value f)
	{
	(void)fun;
	return boolean(f->R->T == type_void);
	}

value op_is_type(value fun, value f, type t)
	{
	value x = arg(f->R);
	f = boolean(x->T == t);
	drop(x);
	(void)fun;
	return f;
	}

value type_is_void(value fun, value f)
	{
	return op_is_type(fun,f,type_void);
	}

static value op_predicate(value fun, value f, int op(value))
	{
	value x = arg(f->R);
	f = boolean(op(x));
	drop(x);
	(void)fun;
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

value type_is_good(value fun, value f) { return op_predicate(fun,f,is_good); }
value type_is_bool(value fun, value f) { return op_predicate(fun,f,is_bool); }
value type_is_list(value fun, value f) { return op_predicate(fun,f,is_list); }

// \chain=(\\a\\b \x \v=(a x) is_defined v v (b x))
// Return (a x) if that is defined, otherwise return (b x).
value type_chain(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	if (fun->L->L == 0) return keep(fun,f);
	{
	value x = arg(f->R);
	value a = (fun->L->R = eval(fun->L->R));
	value v = eval(A(hold(a),x));

	if (v->T != type_void)
		return v;

	drop(v);
	{
	value b = (fun->R = eval(fun->R));
	return A(hold(b),hold(x));
	}
	}
	}

// Create a nullary function, supplying a dummy argument.
value Q0(type T)
	{
	return A(Q(T),hold(QI));
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
	Qyield = Q(type_yield);
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
	drop(Qyield);
	}
