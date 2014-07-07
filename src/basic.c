#include <value.h>
#include <basic.h>

/* C x y = x */
void type_C(value f)
	{
	if (f->L->L)
		replace(f,f->L->R);
	}

/* S x y z = x z (y z) */
void type_S(value f)
	{
	if (f->L->L && f->L->L->L)
		replace_A(f,
			A(hold(f->L->L->R),hold(f->R)),
			A(hold(f->L->R),hold(f->R)));
	}

/* I x = x */
void type_I(value f)
	{
	replace(f,f->R);
	}

/* F x = I. In other words, F x y = y. */
void type_F(value f)
	{
	replace(f,I);
	}

/* R x y z = x (y z) */
void type_R(value f)
	{
	if (f->L->L && f->L->L->L)
		replace_A(f, hold(f->L->L->R), A(hold(f->L->R),hold(f->R)));
	}

/* L x y z = x z y */
void type_L(value f)
	{
	if (f->L->L && f->L->L->L)
		replace_A(f, A(hold(f->L->L->R),hold(f->R)), hold(f->L->R));
	}

/* Y x = x (Y x) */
void type_Y(value f)
	{
	replace_A(f, hold(f->R), A(hold(f->L),hold(f->R)));
	}

/* void x = void */
void type_void(value f)
	{
	replace_void(f);
	}

void replace_void(value f)
	{
	replace_V(f,type_void,0,0);
	}

void replace_boolean(value f, int x)
	{
	replace_V(f,(x ? type_C : type_F),0,0);
	}

value C;
value S;
value I;
value F;
value R;
value L;
value Y;

void beg_basic(void)
	{
	C = Q(type_C);
	S = Q(type_S);
	I = Q(type_I);
	F = Q(type_F);
	R = Q(type_R);
	L = Q(type_L);
	Y = Q(type_Y);
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
	}
