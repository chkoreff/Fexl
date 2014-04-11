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

value C;
value I;
value S;
value L;
value R;
value Y;
value query;

void beg_base(void)
	{
	C = Q(type_C);
	I = Q(type_I);
	S = Q(type_S);
	L = Q(type_L);
	R = Q(type_R);
	Y = Q(type_Y);
	query = Q(type_query);

	hold(C);
	hold(I);
	hold(S);
	hold(L);
	hold(R);
	hold(Y);
	hold(query);
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
	}
