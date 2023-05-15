#include <stdio.h>
#include <io.h>

#include <value.h>
#include <app.h>
#include <lam.h>
#include <ref.h>

#include <show.h>

static unsigned long max_depth;
static unsigned long max_call;

static void limit_show(value exp)
	{
	if (max_call == 0 || max_depth == 0)
		{
		put_ch('_');
		return;
		}

	max_call--;
	max_depth--;

	if (exp->type == &type_ref)
		printf("(R %lu)", exp->ref.pos);
	else if (exp->type == &type_lam)
		{
		put("(L ");
		limit_show(exp->lam.body);
		put_ch(')');
		}
	else if (exp->type == &type_app)
		{
		put("(A ");
		limit_show(exp->app.fun);
		put_ch(' ');
		limit_show(exp->app.arg);
		put_ch(')');
		}
	else
		put_ch('?');
	}

void show(value f)
	{
	max_depth = 100;
	max_call = 200;
	limit_show(f);
	}

void show_line(const char *name, value exp)
	{
	put(name); show(exp); nl();
	}
