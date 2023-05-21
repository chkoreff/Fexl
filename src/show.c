#include <stdio.h>
#include <str.h>
#include <output.h>

#include <value.h>
#include <app.h>
#include <lam.h>
#include <ref.h>
#include <basic.h>

#include <type_num.h>
#include <type_str.h>
#include <type_output.h>

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

	if (exp->T == &type_ref)
		printf("(R %lu)", exp->v_u64);
	else if (exp->T == &type_lam)
		{
		put("(L ");
		limit_show(exp->L);
		put_ch(')');
		}
	else if (exp->T == &type_eager)
		{
		put("(E ");
		limit_show(exp->L);
		put_ch(')');
		}
	else if (exp->T == &type_app)
		{
		put("(A ");
		limit_show(exp->L);
		put_ch(' ');
		limit_show(exp->R);
		put_ch(')');
		}
	else if (exp->T == &type_num)
		{
		put("(num ");
		put_double(exp->v_double);
		put_ch(')');
		}
	else if (exp->T == &type_str)
		{
		put("(str ");
		put_quote(exp->v_ptr);
		put_ch(')');
		}
	else if (exp->T == &type_I)
		put("I");
	else if (exp->T == &type_void)
		put("void");
	else if (exp->T == &type_concat)
		put(".");
	else if (exp->T == &type_say)
		put("say");
	else if (exp->T == &type_num_str)
		put("num_str");
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
