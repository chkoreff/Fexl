#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <output.h>
#include <type_form.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_meta.h>
#include <type_num.h>
#include <type_output.h>
#include <type_record.h>
#include <type_str.h>
#include <type_var.h>

#include <show.h>

static void put_quote(string x)
	{
	put_ch('"');
	put_str(x);
	put_ch('"');
	}

static void show_items(value f)
	{
	while (f->T == type_list)
		{
		show(f->L);
		f = f->R;
		if (f->T == type_list)
			put_ch(' ');
		}
	if (f->T != type_null)
		{
		put("; ");
		show(f);
		}
	}

static void show_pairs(value f)
	{
	put_ch('[');
	while (f->L)
		{
		put_ch('{');
		put_quote(f->L->L->v_ptr);
		put_ch(' ');
		show(f->L->R);
		put_ch('}');
		f = f->R;
		if (f->L)
			put_ch(' ');
		}
	put_ch(']');
	}

static void show_record(value f)
	{
	put("(record ");
	show_pairs(f);
	put_ch(')');
	}

static const char *type_name(type t)
	{
	if (t == 0) return "A";
	if (t == type_I) return "I";
	if (t == type_T) return "T";
	if (t == type_F) return "F";
	if (t == type_Y) return "@";
	if (t == type_void) return "void";
	if (t == type_D) return "D";
	if (t == type_E) return "E";
	if (t == type_null) return "[]";
	if (t == type_single) return "single";
	if (t == type_pair) return "pair";
	if (t == type_form) return "form";
	if (t == type_def) return "def";
	if (t == type_set) return "set";
	if (t == type_get) return "get";
	if (t == type_chain) return "::";
	if (t == type_record_pairs) return "record_pairs";
	if (t == type_restrict) return "restrict";
	if (t == type_cons) return "cons";
	if (t == type_say) return "say";
	if (t == type_put) return "put";
	if (t == type_nl) return "nl";
	if (t == type_limit_time) return "limit_time";
	if (t == type_limit_stack) return "limit_stack";
	if (t == type_limit_memory) return "limit_memory";
	if (t == type_show_benchmark) return "show_benchmark";
	if (t == type_trace_benchmark) return "trace_benchmark";
	if (t == type_add) return "+";
	if (t == type_pow) return "^";
	if (t == type_num_str) return "num_str";
	if (t == type_read) return "read";
	if (t == type_show) return "show";
	if (t == type_concat) return ".";
	return "?";
	}

void show(value f)
	{
	if (f->T == type_num)
		put_double(f->v_double);
	else if (f->T == type_str)
		put_quote(f->v_ptr);
	else if (f->T == type_list)
		{
		put_ch('[');
		show_items(f);
		put_ch(']');
		}
	else if (f->T == type_tuple)
		{
		put_ch('{');
		show_items(f->R);
		put_ch('}');
		}
	else if (f->T == type_var)
		{
		put("(var "); show(f->R); put(")");
		}
	else if (f->T == type_record)
		show_record(f);
	else
		{
		const char *name = type_name(f->T);
		if (f->L == 0)
			put(name);
		else
			{
			put_ch('(');
			put(name);
			put_ch(' '); show(f->L);
			put_ch(' '); show(f->R);
			put_ch(')');
			}
		}
	}

void show_line(const char *name, value f)
	{
	put(name); show(f); nl();
	}
