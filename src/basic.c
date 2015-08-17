#include <value.h>
#include <basic.h>

/* (C x y) = x */
value type_C(value f)
	{
	if (!f->L || !f->L->L) return 0;
	reduce(f,hold(f->L->R));
	return f;
	}

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
	if (!f->L) return 0;
	f->T = type_C;
	return 0;
	}

value type_F(value f)
	{
	if (!f->L) return 0;
	reduce_Q(f,type_I);
	return 0;
	}

/* (Y x) = (x (Y x)) */
value type_Y(value f)
	{
	if (!f->L) return 0;
	reduce_A(f, hold(f->R), A(hold(f->L),hold(f->R)));
	return f;
	}

/* (? x f) = (f v), where v is the value of x. */
value type_query(value f)
	{
	if (!f->L || !f->L->L) return 0;
	reduce_A(f, hold(f->R), hold(arg(f->L->R)));
	return f;
	}

/* (once x) = x, but x is evaluated only once. */
value type_once(value f)
	{
	if (!f->L) return 0;
	reduce(f,hold(arg(f->R)));
	return f;
	}

/* (void x) = void */
value type_void(value f)
	{
	if (!f->L) return 0;
	reduce_void(f);
	return 0;
	}

/* (single x p) = (p x) */
value type_single(value f)
	{
	if (!f->L || !f->L->L) return 0;
	reduce_A(f, hold(f->R), hold(f->L->R));
	return f;
	}

/* (cons x y A B) = (B x y) */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	reduce_A(f, A(hold(f->R),hold(f->L->L->L->R)), hold(f->L->L->R));
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
	reduce_boolean(f, x->T != type_void);
	return 0;
	}
	}

value type_is_bool(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	reduce_boolean(f, x->T == type_T || x->T == type_F);
	return 0;
	}
	}

value type_is_list(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	reduce_boolean(f, x->T == type_null
		|| (x->T == type_cons && x->L && x->L->L));
	return 0;
	}
	}

value op_is_type(value f, type t)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	reduce_boolean(f, x->T == t);
	return 0;
	}
	}

void reduce_void(value f)
	{
	reduce_Q(f,type_void);
	}

void reduce_boolean(value f, int x)
	{
	reduce_Q(f, x ? type_T : type_F);
	}

value single(value x)
	{
	return A(Q(type_single), x);
	}
