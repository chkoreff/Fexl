#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <output.h>
#include <type_num.h>
#include <type_str.h>

#include <show.h>

// Find the name of atom f in context cx.
static const char *atom_name(value f, value cx)
	{
	while (1)
		{
		if (cx->T == &type_A)
			{
			value top = cx->L;
			value val = top->R;
			if (f->T == val->T && f->v_ptr == val->v_ptr)
				return str_data(top->L);
			cx = cx->R;
			}
		else
			return "?";
		}
	}

static void put_quote(string x)
	{
	put_ch('"');
	put_str(x);
	put_ch('"');
	}

static void limit_show(value f);

void show_exp(const char *name, value f)
	{
	put_ch('(');
	put(name);
	put_ch(' '); limit_show(f->L);
	put_ch(' '); limit_show(f->R);
	put_ch(')');
	}

static unsigned long max_depth;
static unsigned long max_call;

static void limit_show(value f)
	{
	if (max_call == 0 || max_depth == 0)
		{
		put_ch('_');
		return;
		}

	max_call--;
	max_depth--;

	if (f->T == &type_num)
		put_double(f->v_double);
	else if (f->T == &type_str)
		put_quote(get_str(f));
	else
		{
		if (f->T == &type_A)
			show_exp("A",f);
		else if (f->T == &type_L)
			show_exp("L",f);
		else if (f->T == &type_E)
			show_exp("E",f);
		// LATER Use special list syntax.
		else if (f->T == &type_list)
			show_exp("list",f);
		else if (f->T == &type_null)
			{
			if (f->L)
				show_exp("?",f); // intermediate form
			else
				put("null");
			}
		else if (f->L)
			show_exp("?",f); // intermediate form
		else
			put(atom_name(f,cx_std)); // atom
		}

	max_depth++;
	}

void show(value f)
	{
	max_depth = 12;
	max_call = 200;
	limit_show(f);
	}

void show_line(const char *name, value f)
	{
	put(name);show(f);nl();
	}
