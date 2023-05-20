#include <value.h>
#include <app.h>
#include <ref.h>

#include <lam.h>

static value apply(value fun, value arg, value cx)
	{
	// Don't bind a reference to a reference.  This allows infinite loops
	// to run in constant space.
	value pair =
		(arg->type == &type_ref)
			? find_pos(arg->v_u64,cx)
			: A(hold(arg),hold(cx));

	value fun_exp = fun->L;
	value fun_cx = fun->R;

	value body_exp = hold(fun_exp->L);
	value body_cx = A(pair,hold(fun_cx));

	return A(body_exp, body_cx);
	}

static void clear(value exp)
	{
	drop(exp->L);
	}

struct type type_lam = { no_step, apply, clear };

value L(value body)
	{
	value exp = new_exp(&type_lam);
	exp->L = body;
	return exp;
	}
