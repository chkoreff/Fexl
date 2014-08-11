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

/* F x = I. In other words, F x y = y. */
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

/* (cons x y A) = (pair x y) */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(A(Q(type_pair),hold(f->L->L->R)),hold(f->L->R));
	}

/* Yields a type error. */
value type_bad(value f)
	{
	(void)f;
	return 0;
	}

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

/* (catch x ok err) = (ok x) if x evaluates successfully, otherwise err. */
value type_catch(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	{
	value g = 0;
	value x = eval(hold(f->L->L->R));
	if (x)
		g = apply(hold(f->L->R),x);
	else
		{
		g = hold(f->R);
		drop(x);
		}
	return g;
	}
	}

value Qboolean(int x)
	{
	return hold(x ? C : F);
	}

value Qis_atom(type t, value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	value g = Qboolean(is_atom(t,x));
	drop(x);
	return g;
	}
	}

value C;
value S;
value I;
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
	drop(F);
	drop(R);
	drop(L);
	drop(Y);
	drop(Qeval);
	drop(Qcons);
	}
