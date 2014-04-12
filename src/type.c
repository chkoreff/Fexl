#include <value.h>
#include <type.h>

value type_C(value f, value g)
	{
	if (!f->L) return collect(f,g);
	return f->R;
	}

value type_I(value f, value g)
	{
	(void)f;
	return g;
	}

value type_S(value f, value g)
	{
	if (!f->L || !f->L->L) return collect(f,g);
	return combine(combine(f->L->R,g),A(f->R,g));
	}

value type_L(value f, value g)
	{
	if (!f->L || !f->L->L) return collect(f,g);
	return combine(combine(f->L->R,g),f->R);
	}

value type_R(value f, value g)
	{
	if (!f->L || !f->L->L) return collect(f,g);
	return combine(f->L->R,A(f->R,g));
	}

value type_Y(value f, value g)
	{
	return combine(g,A(f,g));
	}

/* (query x y) = y x, except x is evaluated first. */
value type_query(value f, value g)
	{
	if (!f->L) return collect(f,g);
	value x = eval(f->R);
	value z = combine(g,x);
	drop(x);
	return z;
	}

/* pair x y F = F x y */
value type_pair(value f, value g)
	{
	if (!f->L || !f->L->L) return collect(f,g);
	return A(A(g,f->L->R),f->R);
	}

/* (cons x y) F G = G x y */
value type_cons(value f, value g)
	{
	if (!f->L || !f->L->L) return collect(f,g);
	return pair(f->L->R,f->R);
	}

value pair(value x, value y)
	{
	return V(type_pair,V(type_pair,Qpair,x),y);
	}

value cons(value x, value y)
	{
	return V(type_cons,V(type_cons,Qcons,x),y);
	}

value C;
value I;
value S;
value L;
value R;
value Y;
value query;
value Qpair;
value Qcons;

void beg_base(void)
	{
	C = Q(type_C);
	I = Q(type_I);
	S = Q(type_S);
	L = Q(type_L);
	R = Q(type_R);
	Y = Q(type_Y);
	query = Q(type_query);
	Qpair = Q(type_pair);
	Qcons = Q(type_cons);

	hold(C);
	hold(I);
	hold(S);
	hold(L);
	hold(R);
	hold(Y);
	hold(query);
	hold(Qpair);
	hold(Qcons);
	}

void end_base(void)
	{
	drop(C);
	drop(I);
	drop(S);
	drop(L);
	drop(R);
	drop(Y);
	drop(query);
	drop(Qpair);
	drop(Qcons);
	}
