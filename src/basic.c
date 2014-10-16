#include <value.h>
#include <basic.h>

/* (C x y) = x */
value type_C(value f)
	{
	if (!f->L || !f->L->L) return f;
	return hold(f->L->R);
	}

/* (S x y z) = ((x z) (y z)) */
value type_S(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return apply(apply(hold(f->L->L->R),hold(f->R)),
		A(hold(f->L->R),hold(f->R)));
	}

/* (I x) = x */
value type_I(value f)
	{
	if (!f->L) return f;
	return hold(f->R);
	}

/* Boolean types:
(T x y) = x
(F x y) = y
*/

value type_T(value f)
	{
	return type_C(f);
	}

value type_F(value f)
	{
	if (!f->L) return f;
	return hold(I);
	}

/* (R x y z) = (x (y z)) */
value type_R(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return apply(hold(f->L->L->R),A(hold(f->L->R),hold(f->R)));
	}

/* (L x y z) = ((x z) y) */
value type_L(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return apply(apply(hold(f->L->L->R),hold(f->R)),hold(f->L->R));
	}

/* (Y x) = (x (Y x)) */
value type_Y(value f)
	{
	if (!f->L) return f;
	return apply(hold(f->R),hold(f));
	}

/* (pair x y A) = (A x y) */
value type_pair(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return apply(apply(hold(f->R),hold(f->L->L->R)),hold(f->L->R));
	}

/*LATER distinguish between C and null */

/* (cons x y A) = (pair x y) */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(A(Q(type_pair),hold(f->L->L->R)),hold(f->L->R));
	}

/* (void x) = void */
value type_void(value f)
	{
	if (!f->L) return f;
	return hold(f->L);
	}

value type_is_void(value f)
	{
	return Qis_atom(type_void,f);
	}

/* LATER is_good */

/* (eval x y) = (y x), except x is evalated first. */
value type_eval(value f)
	{
	if (!f->L || !f->L->L) return f;
	{
	value x = eval(hold(f->L->R));
	return apply(hold(f->R),x);
	}
	}

/* (later x) = x, except that x is evaluated only if it's called later. */
value type_later(value f)
	{
	if (!f->L) return f;
	drop(f->L);
	f->L = hold(I);
	f->T = type_A;
	return f;
	}

value type_is_bool(value f)
	{
	if (!f->L) return f;
	return Qboolean(f->R->T == type_T || f->R->T == type_F);
	}

value Qboolean(int x)
	{
	return hold(x ? T : F);
	}

value Qis_atom(type t, value f)
	{
	if (!f->L) return f;
	return Qboolean(f->R->T == t);
	}

/* LATER is_list */

value C;
value S;
value I;
value T;
value F;
value R;
value L;
value Y;
value Qeval;
value Qcons;

void beg_basic(void)
	{
	C = Q(type_C);
	S = Q(type_S);
	I = Q(type_I);
	T = Q(type_T);
	F = Q(type_F);
	R = Q(type_R);
	L = Q(type_L);
	Y = Q(type_Y);
	Qeval = Q(type_eval);
	Qcons = Q(type_cons);
	}

void end_basic(void)
	{
	drop(C);
	drop(S);
	drop(I);
	drop(T);
	drop(F);
	drop(R);
	drop(L);
	drop(Y);
	drop(Qeval);
	drop(Qcons);
	}
