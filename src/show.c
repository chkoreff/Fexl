#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <output.h>
#include <parse.h>
#include <type_num.h>
#include <type_str.h>
#include <type_var.h>

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

void show_exp(const char *name, value f)
	{
	put_ch('(');
	put(name);
	put_ch(' '); show(f->L);
	put_ch(' '); show(f->R);
	put_ch(')');
	}

void show(value f)
	{
	if (f->T == &type_num)
		put_double(f->v_double);
	else if (f->T == &type_str)
		put_quote(get_str(f));
	else if (f->T == &type_A)
		show_exp("A",f);
	else if (f->T == &type_D)
		show_exp("D",f);
	else if (f->T == &type_E)
		show_exp("E",f);
	else if (f->T == &type_list)
		show_exp("list",f);
	else if (f->T == &type_null && f->L == 0)
		put("null");
	else if (f->T == &type_tuple)
		show_exp("tuple",f);
	else if (f->T == &type_single)
		show_exp("single",f);
	else if (f->T == &type_pair)
		show_exp("pair",f);
	else if (f->T == &type_form)
		show_exp("form",f);
	else if (f->T == &type_var)
		{
		put("(var "); show(f->R); put(")");
		}
	else if (f->L)
		show_exp("?",f); // intermediate value
	else
		put(atom_name(f,cx_cur)); // atom
	}

void show_line(const char *name, value f)
	{
	put(name);show(f);nl();
	}
