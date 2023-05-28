#include <value.h>
#include <context.h>

#include <basic.h>

value QI;
value QT;
value QF;
value QY;
value Qvoid;

// (I x) = x
static value apply_I(value f, value x)
	{
	drop(f);
	return x;
	}

static struct type type_I = { 0, apply_I, no_clear };

// (T x y) = x
static value apply_T(value f, value x)
	{
	if (f->L == 0)
		return V(f->T,f,x);
	else
		{
		value g = hold(f->R);
		drop(f);
		drop(x);
		return g;
		}
	}

void clear_T(value f)
	{
	if (f->L)
		{
		drop(f->L);
		drop(f->R);
		}
	}

static struct type type_T = { 0, apply_T, clear_T };

// (F x y) = y
static value apply_F(value f, value x)
	{
	drop(f);
	drop(x);
	return hold(QI);
	}

static struct type type_F = { 0, apply_F, no_clear };

// (Y x) = (x (Y x))
static value apply_Y(value f, value x)
	{
	return A(x,A(f,hold(x)));
	}

static struct type type_Y = { 0, apply_Y, no_clear };

// (void x) = void
value apply_void(value f, value x)
	{
	drop(f);
	drop(x);
	return hold(Qvoid);
	}

static struct type type_void = { 0, apply_void, no_clear };

// Use pattern p to make a copy of expression e with argument x substituted in
// the places designated by the pattern.
static value subst(value p, value e, value x)
	{
	if (p == QF) return hold(e);
	if (p == QT) return hold(x);
	return V(e->T,subst(p->L,e->L,x),subst(p->R,e->R,x));
	}

// Lambda substitution
static value apply_L(value f, value x)
	{
	value g = subst(f->L,f->R,x);
	drop(f);
	drop(x);
	return g;
	}

// Eager substitution
static value apply_E(value f, value x)
	{
	return apply_L(f,eval(x));
	}

struct type type_L = { 0, apply_L, clear_A };
struct type type_E = { 0, apply_E, clear_A };

void beg_basic(void)
	{
	QI = Q(&type_I,0);
	QT = Q(&type_T,0);
	QF = Q(&type_F,0);
	QY = Q(&type_Y,0);
	Qvoid = Q(&type_void,0);
	}

void def_basic(void)
	{
	define("I", hold(QI));
	define("T", hold(QT));
	define("F", hold(QF));
	define("@", Q(&type_Y,0));
	define("void", hold(Qvoid));
	}

void end_basic(void)
	{
	drop(QI);
	drop(QT);
	drop(QF);
	drop(QY);
	drop(Qvoid);
	}
