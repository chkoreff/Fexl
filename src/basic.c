#include <value.h>
#include <context.h>

#include <basic.h>

value QI;
value QT;
value QF;
value Qvoid;
value Qnull;

value keep(value f, value x)
	{
	return V(f->T,hold(f),x);
	}

value need(value f, value x, type T)
	{
	x = eval(x);
	if (x->T == T)
		return keep(f,x);
	else
		{
		drop(x);
		return hold(Qvoid);
		}
	}

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
		return keep(f,x);
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
value subst(value p, value e, value x)
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

static value apply_yield(value f, value x)
	{
	(void)f;
	return single(x);
	}

static struct type type_yield = { 0, apply_yield, no_clear };

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
		return keep(f,x);
	else
		return V(&type_list,hold(f->R),x);
	}

static struct type type_cons = { 0, apply_cons, clear_T };

// Tuples

static value apply_tuple(value f, value x)
	{
	value list = hold(f->R);
	while (1)
		{
		list = eval(list);
		if (list->T == &type_null)
			{
			drop(list);
			return x;
			}
		else if (list->T == &type_list)
			{
			value item = hold(list->L);
			value tail = hold(list->R);
			drop(list);
			x = A(x,item);
			list = tail;
			}
		else
			return A(x,list); // improper list tail
		}
	}

struct type type_tuple = { 0, apply_tuple, clear_A };

// Convert list to tuple, e.g. [1 2 3] to {1 2 3}.
static value apply_list_to_tuple(value f, value x)
	{
	(void)f;
	return V(&type_tuple,hold(QI),x);
	}

struct type type_list_to_tuple = { 0, apply_list_to_tuple, clear_T };

// Convert to tuple to list, e.g. {1 2 3} to [1 2 3].
static value apply_tuple_to_list(value f, value x)
	{
	x = eval(x);
	if (x->T == &type_tuple)
		f = hold(x->R);
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

struct type type_tuple_to_list = { 0, apply_tuple_to_list, clear_T };

void beg_basic(void)
	{
	QI = Q(&type_I,0);
	QT = Q(&type_T,0);
	QF = Q(&type_F,0);
	Qvoid = Q(&type_void,0);
	Qnull = Q(&type_null,0);
	}

void use_basic(void)
	{
	define("I", hold(QI));
	define("T", hold(QT));
	define("F", hold(QF));
	define("@", Q(&type_Y,0));
	define("void", hold(Qvoid));
	define("null", hold(Qnull));
	define("yield", Q(&type_yield,0));
	define("cons", Q(&type_cons,0));
	define("list_to_tuple", Q(&type_list_to_tuple,0));
	define("tuple_to_list", Q(&type_tuple_to_list,0));
	}

void end_basic(void)
	{
	drop(QI);
	drop(QT);
	drop(QF);
	drop(Qvoid);
	drop(Qnull);
	}
