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

static const char *type_name(type t)
	{
	if (t == 0) return "A";
	if (t == type_num) return "num";
	if (t == type_str) return "str";

	if (t == type_form) return "form";
	if (t == type_quo) return "quo";
	if (t == type_ref) return "ref";

	if (t == type_T) return "T";
	if (t == type_F) return "F";
	if (t == type_I) return "I";
	if (t == type_Y) return "Y";
	if (t == type_once) return "once";
	if (t == type_void) return "void";
	if (t == type_yield) return "yield";
	if (t == type_concat) return "concat";
	if (t == type_say) return "say";
	if (t == type_add) return "add";
	if (t == type_mul) return "mul";
	if (t == type_list) return "list";
	if (t == type_D) return "D";
	if (t == type_E) return "E";
	if (t == type_pair) return "pair";
	if (t == type_null) return "null";
	if (t == type_tuple) return "tuple";
	if (t == type_assoc) return "assoc";
	if (t == type_with) return "with";
	if (t == type_chain) return "chain";

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
		value x = f->R;
		put_quote(x->v_ptr);
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
	put(type_name(f->T));
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

static value type_show(value f)
	{
	show_exp(f->R);nl();
	return hold(QI);
	}

static value define(void)
	{
	if (match("show")) return Q(type_show);
	return 0;
	}

value type_cx_show(value f)
	{
	return op_context(f,define);
	}
