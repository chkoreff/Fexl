#include <stdio.h>
#include <str.h>
#include <io.h>

#include <value.h>
#include <app.h>
#include <lam.h>
#include <ref.h>

#include <type_str.h>

#include <show.h>

static unsigned long max_depth;
static unsigned long max_call;

static void put_quote(string x)
	{
	put_ch('"');
	put_str(x);
	put_ch('"');
	}

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
	else if (exp->type == &type_str)
		{
		put("(str ");
		put_quote(exp->v_ptr);
		put_ch(')');
		}
	else
		put_ch('?');

	max_depth++;
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
