#include <value.h>
#include <basic.h>

/* (C x y) = x */
value type_C(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return hold(f->L->R);
	}

/* (I x) = x */
value type_I(value f)
	{
	if (!f->L) return 0;
	return hold(f->R);
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
	return hold(I);
	}

/* (Y x) = (x (Y x)) */
value type_Y(value f)
	{
	if (!f->L) return 0;
	return A(hold(f->R),hold(f));
	}

/* (? x next) = (next y), where y is the final value of x. */
value type_query(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = eval(hold(f->L->R));
	return A(hold(f->R),x);
	}
	}

/* (void x) = void */
value type_void(value f)
	{
	if (!f->L) return 0;
	return hold(f->L);
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
	if (!f->L) return 0;
	f->T = type_C;
	return 0;
	}

value type_is_void(value f)
	{
	return op_is_type(f,type_void);
	}

value type_is_good(value f)
	{
	if (!f->L) return 0;
	return Qboolean(f->R->T != type_void);
	}

value type_is_bool(value f)
	{
	if (!f->L) return 0;
	{
	value x = f->R;
	return Qboolean(x->T == type_T || x->T == type_F);
	}
	}

value type_is_list(value f)
	{
	if (!f->L) return 0;
	{
	value x = f->R;
	return Qboolean(x->T == type_null ||
		(x->L && x->L->L && x->L->L->T == type_cons));
	}
	}

value op_is_type(value f, type t)
	{
	if (!f->L) return 0;
	return Qboolean(f->R->T == t);
	}

/*LATER We'll soon eliminate all the functions below.*/

/* (later x) = x, except that x is evaluated only if it's called later. */
value type_later(value f)
	{
	if (!f->L) return 0;
	drop(f->L);
	f->L = hold(I);
	f->T = type_A;
	return 0;
	}

/* Used by type_once. */
static value type_replace_right(value f)
	{
	value x = eval(f->R);
	drop(f->L);
	f->L = hold(I);
	f->R = x;
	f->T = type_A;
	return hold(x);
	}

/* (once x) = x, but x is only evaluated once, on demand */
value type_once(value f)
	{
	if (!f->L) return 0;
	f->T = type_replace_right;
	return 0;
	}

value Qboolean(int x)
	{
	return hold(x ? T : F);
	}

value C;
value I;
value T;
value F;
value Y;
value query;
value cons;
value null;

void beg_basic(void)
	{
	C = Q(type_C);
	I = Q(type_I);
	T = Q(type_T);
	F = Q(type_F);
	Y = Q(type_Y);
	query = Q(type_query);
	cons = Q(type_cons);
	null = Q(type_null);
	}

void end_basic(void)
	{
	drop(C);
	drop(I);
	drop(T);
	drop(F);
	drop(Y);
	drop(query);
	drop(cons);
	drop(null);
	}
