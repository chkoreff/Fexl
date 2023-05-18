#include <value.h>

#include <app.h>

static value step(value pair)
	{
	value exp = pair->app.fun;
	value cx = pair->app.arg;
	value fun = eval(A(hold(exp->app.fun),hold(cx)));
	value arg = exp->app.arg;
	value next = fun->app.fun->type->apply(fun,arg,cx);
	drop(fun);
	return next;
	}

static void clear(value exp)
	{
	drop(exp->app.fun);
	drop(exp->app.arg);
	}

struct type type_app = { step, no_apply, clear };

value A(value fun, value arg)
	{
	value exp = new_exp(&type_app);
	exp->app.fun = fun;
	exp->app.arg = arg;
	return exp;
	}
