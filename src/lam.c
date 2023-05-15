#include <value.h>
#include <app.h>
#include <ref.h>

#include <lam.h>

static value step(value pair)
	{
	(void)pair;
	return 0;
	}

static value apply(value fun, value arg, value cx)
	{
	// Don't bind a reference to a reference.  This allows infinite loops
	// to run in constant space.
	value pair =
		(arg->type == &type_ref)
			? find_pos(arg->ref.pos,cx)
			: A(hold(arg),hold(cx));

	value fun_exp = fun->app.fun;
	value fun_cx = hold(fun->app.arg);

	return A(hold(fun_exp->lam.body), A(pair,fun_cx));
	}

static void clear(value exp)
	{
	drop(exp->lam.body);
	}

struct type type_lam = { step, apply, clear };

value L(value body)
	{
	value exp = new_exp(&type_lam);
	exp->lam.body = body;
	return exp;
	}
