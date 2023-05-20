#include <value.h>
#include <app.h>
#include <ref.h>

#include <lam.h>

// Don't bind a reference to a reference.  This allows infinite loops to run in
// constant space.
static value get_pair(value arg, value cx)
	{
	return (arg->T == &type_ref)
		? find_pos(arg->v_u64,cx)
		: A(hold(arg),hold(cx));
	}

static value bind(value fun, value pair)
	{
	value fun_exp = fun->L;
	value fun_cx = fun->R;

	value body_exp = hold(fun_exp->L);
	value body_cx = A(pair,hold(fun_cx));

	return A(body_exp, body_cx);
	}

static value apply_lam(value fun, value arg, value cx)
	{
	return bind(fun,get_pair(arg,cx));
	}

static void clear(value exp)
	{
	drop(exp->L);
	}

struct type type_lam = { no_step, apply_lam, clear };

value L(value body)
	{
	value exp = new_exp(&type_lam);
	exp->L = body;
	return exp;
	}
