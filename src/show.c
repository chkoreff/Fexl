#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <file.h>
#include <file_str.h>
#include <show.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_record.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_tuple.h>

static int done;
static type t_Y;
static type t_yield;
static type t_concat;
static type t_say;
static type t_add;
static type t_mul;
static type t_evaluate;

static const char *type_name(type t)
	{
	if (t == 0) return "A";
	if (t == type_num) return "num";
	if (t == type_str) return "str";

	if (t == type_form) return "form";
	if (t == type_quo) return "quo";
	if (t == type_ref) return "ref";

	if (t == type_list) return "list";
	if (t == type_D) return "D";
	if (t == type_E) return "E";
	if (t == type_void) return "void";
	if (t == type_pair) return "pair";
	if (t == type_null) return "null";
	if (t == type_tuple) return "tuple";
	if (t == type_T) return "T";
	if (t == type_F) return "F";
	if (t == QI->T) return "I";
	if (t == Qonce->T) return "once";

	if (t == t_Y) return "Y";
	if (t == t_yield) return "yield";
	if (t == t_concat) return "concat";
	if (t == t_say) return "say";
	if (t == t_add) return "add";
	if (t == t_mul) return "mul";
	if (t == t_evaluate) return "evaluate";

	return "TYPE";
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
		put_quote(f->v_ptr);
	else if (f->T == type_quo)
		limit_show(f->R);
	else if (f->T == type_ref)
		{
		put_quote(f->R->v_ptr);
		if (0) // Don't show the line number.
		{
		put_ch(' ');
		put_ulong(f->R->N);
		}
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
	put(type_name(f->T));
	if (f->L)
		{
		put_ch(' ');
		if (f->L->N)
			{
			if (f->T != type_form) // Don't show the file name.
			{
			limit_show(f->L);
			put_ch(' ');
			}
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

static type get_type(const char *s_key)
	{
	value key = Qstr0(s_key);
	value val = record_find(Qstd,key);
	drop(key);
	return val ? val->T : 0;
	}

value type_show(value f)
	{
	if (!done)
	{
	done = 1;
	t_Y = get_type("@");
	t_yield = get_type("yield");
	t_concat = get_type(".");
	t_say = get_type("say");
	t_add = get_type("+");
	t_mul = get_type("*");
	t_evaluate = get_type("evaluate");
	}

	show_exp(f->R);nl();
	return hold(QI);
	}
