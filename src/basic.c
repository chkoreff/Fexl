#include <value.h>
#include <app.h>
#include <lam.h>

#include <basic.h>

value QI;
value Qvoid;

// (I x) = x
static value step_I(value pair)
	{
	return hold(pair->R->L);
	}

struct type type_I = { step_I, no_apply, no_clear };

// (void x) = void
static value apply_void(value fun, value arg, value cx)
	{
	(void)arg;
	(void)cx;
	return hold(fun);
	}

struct type type_void = { no_step, apply_void, no_clear };

// Return void when atom is used as a function.
value apply_atom(value fun, value arg, value cx)
	{
	(void)fun;
	(void)arg;
	return A(hold(Qvoid),hold(cx));
	}

void beg_basic(void)
	{
	QI = L(new_exp(&type_I));
	Qvoid = new_exp(&type_void);
	}

void end_basic(void)
	{
	drop(QI);
	drop(Qvoid);
	}
