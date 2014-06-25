#include <value.h>

#include <basic.h>

/* C x y = x */
value type_C(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return hold(f->L->R);
	}

/* S x y z = x z (y z) */
value type_S(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return apply(
		apply(hold(f->L->L->R),hold(f->R)),
		A(hold(f->L->R),hold(f->R)));
	}

/* I x = x */
value type_I(value f)
	{
	if (!f->L) return 0;
	return hold(f->R);
	}

/* F x = I. In other words, F x y = y. */
value type_F(value f)
	{
	if (!f->L) return 0;
	return hold(I);
	}

/* R x y z = x (y z) */
value type_R(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return apply(hold(f->L->L->R),A(hold(f->L->R),hold(f->R)));
	}

/* L x y z = x z y */
value type_L(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return apply(apply(hold(f->L->L->R),hold(f->R)),hold(f->L->R));
	}

/* Y x = x (Y x) */
value type_Y(value f)
	{
	if (!f->L) return 0;
	return apply(hold(f->R),hold(f));
	}

/* (query x y) = y x, except x is evaluated first. */
value type_query(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return apply(hold(f->R),arg(f->L->R));
	}

value Qboolean(int x)
	{
	return hold(x ? C : F);
	}

value C;
value S;
value I;
value R;
value L;
value Y;
value F;
value query;

void beg_basic(void)
	{
	C = Q(type_C);
	S = Q(type_S);
	I = Q(type_I);
	R = Q(type_R);
	L = Q(type_L);
	Y = Q(type_Y);
	F = Q(type_F);
	query = Q(type_query);
	}

void end_basic(void)
	{
	drop(C);
	drop(S);
	drop(I);
	drop(R);
	drop(L);
	drop(Y);
	drop(F);
	drop(query);
	}
