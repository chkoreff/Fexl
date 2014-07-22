#include <value.h>
#include <basic.h>

/* (C x y) = x */
value type_C(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return f->L->R;
	}

/* (S x y z) = ((x z) (y z)) */
value type_S(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value z = f->R;
	/* Ensure that the right side is evaluated at most once. */
	if (z->T != type_J)
		{
		z = A(J,z);
		if (!z) return 0;
		z->T = type_J;
		drop(f->R);
		f->R = hold(z);
		}

	return A(A(f->L->L->R,z),A(f->L->R,z));
	}
	}

/* (I x) = x */
value type_I(value f)
	{
	if (!f->L) return 0;
	return f->R;
	}

/* (J x) = x, except x is evaluated only once and replaced on the right. */
value type_J(value f)
	{
	if (!f->L) return 0;
	return arg(&f->R);
	}

/* F x = I. In other words, F x y = y. */
value type_F(value f)
	{
	if (!f->L) return 0;
	return I;
	}

/* (R x y z) = (x (y z)) */
value type_R(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return A(f->L->L->R,A(f->L->R,f->R));
	}

/* (L x y z) = ((x z) y) */
value type_L(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return A(A(f->L->L->R,f->R),f->L->R);
	}

/* (Y x) = (x (Y x)) */
value type_Y(value f)
	{
	if (!f->L) return 0;
	return A(f->R,f);
	}

value Qboolean(int x)
	{
	return x ? C : F;
	}

value C;
value S;
value I;
value J;
value F;
value R;
value L;
value Y;

void beg_basic(void)
	{
	C = hold(Q(type_C));
	S = hold(Q(type_S));
	I = hold(Q(type_I));
	J = hold(Q(type_J));
	F = hold(Q(type_F));
	R = hold(Q(type_R));
	L = hold(Q(type_L));
	Y = hold(Q(type_Y));
	}

void end_basic(void)
	{
	drop(C);
	drop(S);
	drop(I);
	drop(J);
	drop(F);
	drop(R);
	drop(L);
	drop(Y);
	}
