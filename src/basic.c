#include <value.h>
#include <basic.h>

/* (C x y) = x */
value type_C(value f)
	{
	if (!f->L || !f->L->L) return f;
	return hold(f->L->R);
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

/* (Y x) = (x (Y x)) */
value type_Y(value f)
	{
	if (!f->L) return f;
	return apply(hold(f->R),hold(f));
	}

static value substitute(value p, value f, value x)
	{
	if (p->T == type_C)
		return hold(f);
	else if (p->T == type_I)
		return hold(x);
	else if (p->T == type_A)
		return A(substitute(p->L,f->L,x),substitute(p->R,f->R,x));
	else
		return 0;
	}

value type_subst(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return substitute(f->L->L->R,f->L->R,f->R);
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
value I;
value T;
value F;
value Y;
value subst;
value Qeval;
value Qcons;

void beg_basic(void)
	{
	C = Q(type_C);
	I = Q(type_I);
	T = Q(type_T);
	F = Q(type_F);
	Y = Q(type_Y);
	subst = Q(type_subst);
	Qeval = Q(type_eval);
	Qcons = Q(type_cons);
	}

void end_basic(void)
	{
	drop(C);
	drop(I);
	drop(T);
	drop(F);
	drop(Y);
	drop(subst);
	drop(Qeval);
	drop(Qcons);
	}
