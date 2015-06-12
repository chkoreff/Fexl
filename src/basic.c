#include <value.h>
#include <basic.h>

/* (C x y) = x */
value type_C(value f)
	{
	if (!f->L || !f->L->L) return 0;
	replace(f,hold(f->L->R));
	return f;
	}

/* (I x) = x */
value type_I(value f)
	{
	if (!f->L) return 0;
	replace(f,hold(f->R));
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
	replace_Q(f,type_I);
	return 0;
	}

/* (Y x) = (x (Y x)) */
value type_Y(value f)
	{
	if (!f->L) return 0;
	replace_A(f, hold(f->R), A(hold(f->L),hold(f->R)));
	return f;
	}

/* (once x) = x, but x is evaluated only once. */
value type_once(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x != f->R)
		replace(f->R, hold(x));

	drop(f->L);
	f->L = Q(type_I);
	f->T = type_I;
	return x;
	}
	}

/* (void x) = void */
value type_void(value f)
	{
	if (!f->L) return 0;
	replace_void(f);
	return 0;
	}

/* (single x p) = (p x) */
value type_single(value f)
	{
	if (!f->L || !f->L->L) return 0;
	replace_A(f, hold(f->R), hold(f->L->R));
	return f;
	}

/* (cons x y A B) = (B x y) */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	replace_A(f, A(hold(f->R),hold(f->L->L->L->R)), hold(f->L->L->R));
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
	value x = eval(hold(f->R));
	replace_boolean(f, x->T != type_void);
	drop(x);
	return 0;
	}
	}

value type_is_bool(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	replace_boolean(f, x->T == type_T || x->T == type_F);
	drop(x);
	return 0;
	}
	}

value type_is_list(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	replace_boolean(f, x->T == type_null
		|| (x->T == type_cons && x->L && x->L->L));
	drop(x);
	return 0;
	}
	}

value op_is_type(value f, type t)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	replace_boolean(f, x->T == t);
	drop(x);
	return 0;
	}
	}

void replace_void(value f)
	{
	replace_Q(f,type_void);
	}

void replace_boolean(value f, int x)
	{
	replace_Q(f, x ? type_T : type_F);
	}

void replace_single(value f, value x)
	{
	replace_A(f, Q(type_single), x);
	f->T = type_single;
	}
