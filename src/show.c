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

static void limit_show(value f);

static void show_num(value f)
	{
	put("Qnum(");
	put_double(f->v_double);
	put(")");
	}

static void put_quote(string x)
	{
	put_ch('"');
	put_str(x);
	put_ch('"');
	}

static void show_str(value f)
	{
	put("Qstr0(");
	put_quote(get_str(f));
	put(")");
	}

static void show_quo(value f)
	{
	put("quo(");
	limit_show(f->R);
	put_ch(')');
	}

static void show_ref(value f)
	{
	value x = f->R;
	put("ref(");
	put_quote(get_str(x));
	put_ch(',');
	put_ulong(x->N);
	put_ch(')');
	}

static void show_data(value f)
	{
	if (f->T == type_num)
		show_num(f);
	else if (f->T == type_str)
		show_str(f);
	else if (f->T == type_quo)
		show_quo(f);
	else if (f->T == type_ref)
		show_ref(f);
	else
		put("DATA");
	}

static void show_type(type t)
	{
	if (t == type_T) put("type_T");
	else if (t == type_F) put("type_F");
	else if (t == type_I) put("type_I");
	else if (t == type_Y) put("type_Y");
	else if (t == type_once) put("type_once");
	else if (t == type_void) put("type_void");
	else if (t == type_concat) put("type_concat");
	else if (t == type_say) put("type_say");
	else if (t == type_add) put("type_add");
	else if (t == type_mul) put("type_mul");
	else if (t == type_list) put("type_list");
	else if (t == type_D) put("type_D");
	else if (t == type_E) put("type_E");
	else if (t == type_pair) put("type_pair");
	else if (t == type_null) put("type_null");
	else if (t == type_tuple) put("type_tuple");
	else if (t == type_assoc) put("type_assoc");
	else put_ch('_');
	}

static void show_hold(const char *name)
	{
	put("hold(");
	put(name);
	put(")");
	}

static void show_Q(type t)
	{
	put("Q(");
	show_type(t);
	put(")");
	}

static void show_atom(type t)
	{
	if (t == type_T)
		show_hold("QT");
	else if (t == type_F)
		show_hold("QF");
	else if (t == type_I)
		show_hold("QI");
	else if (t == type_Y)
		show_hold("QY");
	else if (t == type_void)
		show_hold("Qvoid");
	else if (t == type_tuple)
		show_hold("Qtuple");
	else
		show_Q(t);
	}

static void show_tree(value f)
	{
	if (f->T == 0)
		put("A(");
	else if (f->T == type_list)
		put("list(");
	else if (f->T == type_form)
		put("Qform(");
	else
		{
		put("V(");
		show_type(f->T);
		put_ch(',');
		}
	limit_show(f->L);
	put_ch(',');
	limit_show(f->R);
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

	if (f->L)
		{
		if (f->L->N)
			show_tree(f);
		else
			show_data(f);
		}
	else
		show_atom(f->T);

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
