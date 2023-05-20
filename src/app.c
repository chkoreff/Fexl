#include <value.h>

#include <app.h>

static value step(value pair)
	{
	value exp = pair->L;
	value cx = pair->R;
	value fun = eval(A(hold(exp->L),hold(cx)));
	value arg = exp->R;
	value next = fun->L->T->apply(fun,arg,cx);
	drop(fun);
	return next;
	}

static void clear(value exp)
	{
	drop(exp->L);
	drop(exp->R);
	}

struct type type_app = { step, no_apply, clear };

value A(value fun, value arg)
	{
	value exp = new_exp(&type_app);
	exp->L = fun;
	exp->R = arg;
	return exp;
	}
