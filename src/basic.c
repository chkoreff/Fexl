#include <value.h>
#include <basic.h>

/* (I x) = x */
value type_I(value f)
	{
	if (!f->L) return 0;
	reduce(f,hold(f->R));
	return f;
	}

/* Boolean types:
(T x y) = x
(F x y) = y
*/
value type_T(value f)
	{
	if (!f->L || !f->L->L) return 0;
	reduce(f,hold(f->L->R));
	return f;
	}

value type_F(value f)
	{
	if (!f->L || !f->L->L) return 0;
	reduce(f,hold(f->R));
	return f;
	}

/* (Y x) = (x (Y x)) */
value type_Y(value f)
	{
	if (!f->L) return 0;
	reduce_A(f,hold(f->R),V(type_Y,hold(f->L),hold(f->R)));
	return f;
	}

/* (eval x) yields the final value of x. */
value type_eval(value f)
	{
	if (!f->L) return 0;
	{
	value x = next_action(f->R);
	if (x) return V(type_eval,hold(f->L),x);
	reduce(f,yield(hold(f->R)));
	return f;
	}
	}

/* (once x) = x, but x is evaluated only once. */
value type_once(value f)
	{
	if (!f->L) return 0;
	reduce(f,arg(f->R));
	return f;
	}

/* (void x) = void */
value type_void(value f)
	{
	if (!f->L) return 0;
	reduce_void(f);
	return 0;
	}

/* (yield x p) = (p x) Used to return a function without evaluating it. */
value type_yield(value f)
	{
	if (!f->L || !f->L->L) return 0;
	reduce_A(f,hold(f->R),hold(f->L->R));
	return f;
	}

/* (cons x y A B) = (B x y) */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	reduce_A(f,A(hold(f->R),hold(f->L->L->L->R)),hold(f->L->L->R));
	return f;
	}

/* (null A B) = A */
value type_null(value f)
	{
	return type_T(f);
	}

value type_is_void(value f)
	{
	return op_is_type(f,type_void);
	}

value type_is_good(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	reduce_boolean(f,(x->T != type_void));
	drop(x);
	return 0;
	}
	}

value type_is_bool(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	reduce_boolean(f,((x->T == type_T || x->T == type_F) && !x->L));
	drop(x);
	return 0;
	}
	}

value type_is_list(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	reduce_boolean(f,((x->T == type_null && !x->L)
		|| (x->T == type_cons && x->L && x->L->L)));
	drop(x);
	return 0;
	}
	}

value op_is_type(value f, type t)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	reduce_boolean(f,(x->T == t));
	drop(x);
	return 0;
	}
	}

value Qvoid(void)
	{
	return Q(type_void);
	}

value QI(void)
	{
	return Q(type_I);
	}

value QF(void)
	{
	return Q(type_F);
	}

value QT(void)
	{
	return Q(type_T);
	}

void reduce_void(value f)
	{
	reduce_Q(f,type_void);
	}

void reduce_boolean(value f, int x)
	{
	reduce_Q(f,(x ? type_T : type_F));
	}

value yield(value x)
	{
	return V(type_yield,Q(type_yield), x);
	}
