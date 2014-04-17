#include <value.h>
#include <basic.h>

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
	return apply(apply(f->L->L->R,f->R),A(f->L->R,f->R));
	}

/* I x = x */
value type_I(value f)
	{
	if (!f->L) return f;
	return f->R;
	}

/* F x = I. In other words, F x y = y. */
value type_F(value f)
	{
	if (!f->L) return f;
	return I;
	}

/* R x y z = x (y z) */
value type_R(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return apply(f->L->L->R,A(f->L->R,f->R));
	}

/* L x y z = x z y */
value type_L(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return apply(apply(f->L->L->R,f->R),f->L->R);
	}

/* Y x = x (Y x) */
value type_Y(value f)
	{
	if (!f->L) return f;
	return apply(f->R,f);
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
	value z = apply(f->R,xp);
	drop(x);
	return z;
	}

/* halt just absorbs any arguments without doing anything. */
value type_halt(value f)
	{
	return f;
	}

/* pair x y F = F x y */
value type_pair(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return apply(apply(f->R,f->L->L->R),f->L->R);
	}

/* (cons x y) F G = G x y */
value type_cons(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return f;
	return apply(apply(f->R,f->L->L->L->R),f->L->L->R);
	}

#if 0
/* (yes x F G) = (G x) */
value type_yes(value f) /*TODO*/
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	return A(f->R,f->L->L->R);
	}
#endif

value pair(value x, value y)
	{
	return V(type_pair,V(type_pair,Qpair,x),y);
	}

value cons(value x, value y)
	{
	return V(type_cons,V(type_cons,Qcons,x),y);
	}

value C;
value S;
value I;
value R;
value L;
value Y;
value query;
value Qpair;
value Qcons;

void beg_basic(void)
	{
	C = Q(type_C);
	S = Q(type_S);
	I = Q(type_I);
	R = Q(type_R);
	L = Q(type_L);
	Y = Q(type_Y);
	query = Q(type_query);
	Qpair = Q(type_pair);
	Qcons = Q(type_cons);

	hold(C);
	hold(S);
	hold(I);
	hold(R);
	hold(L);
	hold(Y);
	hold(query);
	hold(Qpair);
	hold(Qcons);
	}

void end_basic(void)
	{
	drop(C);
	drop(S);
	drop(I);
	drop(R);
	drop(L);
	drop(Y);
	drop(query);
	drop(Qpair);
	drop(Qcons);
	}
