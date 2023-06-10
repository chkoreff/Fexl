#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <output.h>
#include <parse.h>
#include <type_num.h>
#include <type_record.h>
#include <type_str.h>
#include <type_var.h>

#include <show.h>

static value cx_cur;

// TODO possibly look up by value *first* before checking other things.

static value find_value(value f)
	{
	value cx = cx_cur;
	while (cx->L)
		{
		if (f == cx->L->R)
			return cx->L->L;
		cx = cx->R;
		}
	return 0;
	}

// Find the name of atom f in the current context.
static const char *atom_name(value f)
	{
	if (1)
		{
		value val = find_value(f);
		if (val)
			return str_data(val);
		else
			return "?";
		}
	else
		{
		value cx = cx_cur;
		while (cx->L)
			{
			if (f == cx->L->R)
				return str_data(cx->L->L);
			cx = cx->R;
			}
		return "?";
		}
	}

static void put_quote(string x)
	{
	put_ch('"');
	put_str(x);
	put_ch('"');
	}

static void show(value f);

void show_exp(const char *name, value f)
	{
	put_ch('(');
	put(name);
	put_ch(' '); show(f->L);
	put_ch(' '); show(f->R);
	put_ch(')');
	}

static void show_items(value f)
	{
	while (f->T == &type_list)
		{
		show(f->L);
		f = f->R;
		if (f->T == &type_list)
			put_ch(' ');
		}
	if (f->T != &type_null)
		{
		put("; ");
		show(f);
		}
	}

static void show_record(value f)
	{
	if (f->L)
		{
		put_ch('(');
		while (f->L)
			{
			put("set ");
			put_quote(get_str(f->L->L));
			put_ch(' ');
			show(f->L->R);
			put("; ");
			f = f->R;
			}
		put("empty");
		put_ch(')');
		}
	else
		put("empty");
	}

static void show(value f)
	{
	// TODO
	if (f->T == &type_null && f->L == 0)
		{
		put("[]");
		}
	else
	{
	value val = find_value(f);
	if (val)
		{
		put(str_data(val));
		}
	else
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
		{
		put_ch('[');
		show_items(f);
		put_ch(']');
		}
	else if (f->T == &type_null && f->L == 0)
		put("[]");
	else if (f->T == &type_tuple)
		{
		put_ch('{');
		show_items(f->R);
		put_ch('}');
		}
	else if (f->T == &type_single)
		show_exp("single",f);
	else if (f->T == &type_pair)
		show_exp("pair",f);
	else if (f->T == &type_form)
		show_exp("form",f);
	else if (f->T == &type_record)
		show_record(f);
	else if (f->T == &type_var)
		{
		put("(var "); show(f->R); put(")");
		}
	else if (f->L)
		show_exp("?",f); // intermediate value
	else
		put(atom_name(f)); // atom
	}
	}
	}

void show_in(value cx, value f)
	{
	value save_cx_cur = cx_cur;
	cx_cur = cx;
	show(f);
	cx_cur = save_cx_cur;
	}

void show_line(const char *name, value f)
	{
	put(name); show_in(cx_std,f); nl();
	}
