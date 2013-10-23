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
value Qitem;
value yes;

/* C x y = x */
value fexl_C(value f)
	{
	if (!f->L || !f->L->L) return f;
	return f->L->R;
	}

/* S x y z = x z (y z) */
value fexl_S(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(A(f->L->L->R,f->R),A(f->L->R,f->R));
	}

/* I x = x */
value fexl_I(value f)
	{
	if (!f->L) return f;
	return f->R;
	}

/* F x = I.  In other words, F x y = y. */
value fexl_F(value f)
	{
	if (!f->L) return f;
	return I;
	}

value const_fexl_T(void) { return C; }

/* R x y z = x (y z) */
value fexl_R(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(f->L->L->R,A(f->L->R,f->R));
	}

/* L x y z = x z y */
value fexl_L(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(A(f->L->L->R,f->R),f->L->R);
	}

/* Y x = x (Y x) */
value fexl_Y(value f)
	{
	if (!f->L) return f;
	return A(f->R,f);
	}

/* (later x) delays evaluation of x.  It is used to return a function which
should be evaluated later.  The query function recognizes the (later x) form
and peels off the outer "later" layer. */
value fexl_later(value f)
	{
	return f;
	}

/* (query x y) = y x, except x is evaluated first. */
value fexl_query(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value xp = (x->T == fexl_later && x->R) ? x->R : x;
	value z = A(f->R,xp);
	drop(x);
	return z;
	}

/* pair x y F = F x y */
value fexl_pair(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(A(f->R,f->L->L->R),f->L->R);
	}

/* item x y F G = G x y */
value fexl_item(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return f;
	return A(A(f->R,f->L->L->L->R),f->L->L->R);
	}

/* (yes x F G) = (G x) */
value fexl_yes(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(f->R,f->L->L->R);
	}

value const_fexl_no(void) { return C; }

value pair(value x, value y)
	{
	return V(fexl_pair,V(fexl_pair,Q(fexl_pair),x),y);
	}

value item(value x, value y)
	{
	return V(fexl_item,V(fexl_item,Qitem,x),y);
	}

value maybe(value x)
	{
	return x ? V(fexl_yes,yes,x) : C;
	}

void bad_type(void)
	{
	die("You used a data type incorrectly.");
	}

/* Assert that f is an atomic data value of type t. */
void data_type(value f, type t)
	{
	if (f->L == 0 && f->T == t) return;
	bad_type();
	}

void beg_basic(void)
	{
	C = Q(fexl_C); hold(C);
	S = Q(fexl_S); hold(S);
	I = Q(fexl_I); hold(I);
	F = Q(fexl_F); hold(F);
	L = Q(fexl_L); hold(L);
	R = Q(fexl_R); hold(R);
	Y = Q(fexl_Y); hold(Y);
	query = Q(fexl_query); hold(query);
	later = Q(fexl_later); hold(later);
	Qitem = Q(fexl_item); hold(Qitem);
	yes = Q(fexl_yes); hold(yes);
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
	drop(Qitem);
	drop(yes);
	}
