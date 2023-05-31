#include <value.h>
#include <context.h>

#include <basic.h>

value QI;
value QT;
value QF;
value QY;
value Qvoid;
value Qnull;
value Qempty;

// (I x) = x
static value apply_I(value f, value x)
	{
	(void)f;
	return x;
	}

static struct type type_I = { 0, apply_I, no_clear };

// (T x y) = x
static value apply_T(value f, value x)
	{
	if (f->L == 0)
		return V(f->T,hold(f),x);
	else
		{
		drop(x);
		return hold(f->R);
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
	(void)f;
	drop(x);
	return hold(QI);
	}

static struct type type_F = { 0, apply_F, no_clear };

// (Y x) = (x (Y x))
static value apply_Y(value f, value x)
	{
	return A(x,A(hold(f),hold(x)));
	}

static struct type type_Y = { 0, apply_Y, no_clear };

// (void x) = void
value apply_void(value f, value x)
	{
	(void)f;
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

// Direct substitution
static value apply_D(value f, value x)
	{
	f = subst(f->L,f->R,x);
	drop(x);
	return f;
	}

// Eager substitution
static value apply_E(value f, value x)
	{
	return apply_D(f,eval(x));
	}

struct type type_D = { 0, apply_D, clear_A };
struct type type_E = { 0, apply_E, clear_A };

// Single

static value apply_single(value f, value x)
	{
	return A(x,hold(f->R));
	}

struct type type_single = { 0, apply_single, clear_A };

value single(value x)
	{
	return V(&type_single,hold(QI),x);
	}

value maybe(value x)
	{
	if (x == 0)
		return hold(QT);
	else
		return V(&type_T,hold(QT),single(x));
	}

// Pair

static value apply_pair(value f, value x)
	{
	return A(A(x,hold(f->L)),hold(f->R));
	}

struct type type_pair = { 0, apply_pair, clear_A };

// List

struct type type_null = { 0, apply_T, clear_T };

static value apply_list(value f, value x)
	{
	drop(x);
	return V(&type_pair,hold(f->L),hold(f->R));
	}

struct type type_list = { 0, apply_list, clear_A };

static value apply_cons(value f, value x)
	{
	if (f->L == 0)
		return V(f->T,hold(f),x);
	else
		return V(&type_list,hold(f->R),x);
	}

static struct type type_cons = { 0, apply_cons, clear_T };

// Tuples

struct type type_empty = { 0, apply_I, no_clear };

static value apply_tuple(value f, value x)
	{
	value prev = hold(f->L);
	value item = hold(f->R);
	return A(A(prev,x),item);
	}

struct type type_tuple = { 0, apply_tuple, clear_A };

/*
Feed all the items in a list to a function.
In effect this converts a list [x y z] to a tuple {x y z}.
LATER: Do this by converting the list to an actual tuple structure.

\unwind=
	(@\loop\xs\f
	xs f \x\xs
	loop xs (f x)
	)
*/
static value apply_unwind(value f, value x)
	{
	if (f->L == 0)
		return V(f->T,hold(f),x);
	else
		{
		value handler = x;
		value list = eval(hold(f->R));
		if (list->T == &type_null)
			{
			drop(list);
			return handler;
			}
		else if (list->T == &type_list)
			{
			value unwind = hold(f->L);
			value head = hold(list->L);
			value tail = hold(list->R);
			drop(list);
			return A(A(unwind,tail),A(handler,head));
			}
		else
			{
			drop(x);
			drop(list);
			return hold(Qvoid);
			}
		}
	}

struct type type_unwind = { 0, apply_unwind, clear_T };

void beg_basic(void)
	{
	QI = Q(&type_I,0);
	QT = Q(&type_T,0);
	QF = Q(&type_F,0);
	QY = Q(&type_Y,0);
	Qvoid = Q(&type_void,0);
	Qnull = Q(&type_null,0);
	Qempty = Q(&type_empty,0);
	}

void use_basic(void)
	{
	define("I", hold(QI));
	define("T", hold(QT));
	define("F", hold(QF));
	define("@", Q(&type_Y,0));
	define("void", hold(Qvoid));
	define("null", hold(Qnull));
	define("cons", Q(&type_cons,0));
	define("unwind", Q(&type_unwind,0));
	}

void end_basic(void)
	{
	drop(QI);
	drop(QT);
	drop(QF);
	drop(QY);
	drop(Qvoid);
	drop(Qnull);
	drop(Qempty);
	}
