#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <buf.h>
#include <file.h>
#include <file_str.h>
#include <show.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_tuple.h>
#include <type_with.h>

static void put_type(type t)
	{
	if (t == 0) put("A");
	else if (t == type_num) put("num");
	else if (t == type_str) put("str");

	else if (t == type_form) put("form");
	else if (t == type_quo) put("quo");
	else if (t == type_ref) put("ref");

	else if (t == type_T) put("T");
	else if (t == type_F) put("F");
	else if (t == type_I) put("I");
	else if (t == type_Y) put("Y");
	else if (t == type_once) put("once");
	else if (t == type_void) put("void");
	else if (t == type_concat) put("concat");
	else if (t == type_say) put("say");
	else if (t == type_add) put("add");
	else if (t == type_mul) put("mul");
	else if (t == type_list) put("list");
	else if (t == type_D) put("D");
	else if (t == type_E) put("E");
	else if (t == type_pair) put("pair");
	else if (t == type_null) put("null");
	else if (t == type_tuple) put("tuple");
	else if (t == type_assoc) put("assoc");

	else put("TYPE");
	}

// LATER 20231127 Use tilde notation if string has embedded quotes.
static void put_quote(string x)
	{
	put_ch('"');
	put_str(x);
	put_ch('"');
	}

static void limit_show(value f);

static void put_data(value f)
	{
	if (f->T == type_num)
		put_double(f->v_double);
	else if (f->T == type_str)
		put_quote(get_str(f));
	else if (f->T == type_quo)
		limit_show(f->R);
	else if (f->T == type_ref)
		{
		value x = f->R;
		put_quote(get_str(x));
		put_ch(' ');
		put_ulong(x->N);
		}
	else
		put("DATA");
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

	put_ch('[');
	put_type(f->T);
	if (f->L)
		{
		put_ch(' ');
		if (f->L->N)
			{
			limit_show(f->L);
			put_ch(' ');
			limit_show(f->R);
			}
		else
			put_data(f);
		}
	put_ch(']');

	max_depth++;
	}

void show_exp(value f)
	{
	max_depth = 12;
	max_call = 200;
	limit_show(f);
	}

void show(const char *name, value f)
	{
	put(name);show_exp(f);nl();
	}

static value type_show(value fun, value f)
	{
	(void)fun;
	show_exp(f->R);nl();
	return hold(QI);
	}

static value define(void)
	{
	if (match("show")) return Q(type_show);
	return 0;
	}

value type_cx_show(value fun, value f)
	{
	return op_resolve(fun,f,define);
	}
