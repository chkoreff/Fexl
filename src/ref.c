#include <value.h>

#include <ref.h>

value find_pos(unsigned long pos, value cx)
	{
	while (1)
		{
		if (cx->type == &type_ref)
			return 0;
		if (pos == 0)
			return hold(cx->app.fun);
		pos--;
		cx = cx->app.arg;
		}
	}

static value step(value pair)
	{
	return find_pos(pair->app.fun->ref.pos, pair->app.arg);
	}

static value apply(value fun, value arg, value cx)
	{
	(void)fun;
	(void)arg;
	(void)cx;
	return 0;
	}

static void clear(value exp)
	{
	(void)exp;
	}

struct type type_ref = { step, apply, clear };

value R(unsigned long pos)
	{
	value exp = new_exp(&type_ref);
	exp->ref.pos = pos;
	return exp;
	}
