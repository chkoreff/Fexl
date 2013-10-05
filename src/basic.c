#include "die.h"
#include "value.h"
#include "basic.h"

value C;
value S;
value I;
value F;
value L;
value R;
value Y;
value query;
value later;

/* C x y = x */
value type_C(value f)
	{
	if (!f->L || !f->L->L) return f;
	return f->L->R;
	}

/* S x y z = x z (y z) */
value type_S(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(A(f->L->L->R,f->R),A(f->L->R,f->R));
	}

/* I x = x */
value type_I(value f)
	{
	if (!f->L) return f;
	return f->R;
	}

/* F x = I.  In other words, F x y = y. */
value type_F(value f)
	{
	if (!f->L) return f;
	return I;
	}

/* R x y z = x (y z) */
value type_R(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(f->L->L->R,A(f->L->R,f->R));
	}

/* L x y z = x z y */
value type_L(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(A(f->L->L->R,f->R),f->L->R);
	}

/* Y x = x (Y x) */
value type_Y(value f)
	{
	if (!f->L) return f;
	return A(f->R,A(Y,f->R));
	}

/* (later x) delays evaluation of x.  It is used to return a function which
should be evaluated later.  The query function recognizes the (later x) form
and peels off the outer "later" layer. */
value type_later(value f)
	{
	return f;
	}

/* (query x y) = y x, except x is evaluated first. */
value type_query(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value xp = (x->T == type_later && x->R) ? x->R : x;
	value z = A(f->R,xp);
	drop(x);
	return z;
	}

/* (pair x y) = (\: : x y) = (L (L I x) y) */
value pair(value x, value y)
	{
	return A(A(L,A(A(L,I),x)),y);
	}

/* (item x y) = (C (pair x y)) */
value item(value x, value y)
	{
	return A(C,pair(x,y));
	}

void bad_type(void)
	{
	die("You used a data type incorrectly.");
	}

void beg_basic(void)
	{
	C = Q(type_C); hold(C);
	S = Q(type_S); hold(S);
	I = Q(type_I); hold(I);
	F = Q(type_F); hold(F);
	L = Q(type_L); hold(L);
	R = Q(type_R); hold(R);
	Y = Q(type_Y); hold(Y);
	query = Q(type_query); hold(query);
	later = Q(type_later); hold(later);
	}

void end_basic(void)
	{
	drop(C);
	drop(S);
	drop(I);
	drop(F);
	drop(L);
	drop(R);
	drop(Y);
	drop(query);
	drop(later);
	}
