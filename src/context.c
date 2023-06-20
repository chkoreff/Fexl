#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_meta.h>
#include <type_num.h>
#include <type_output.h>
#include <type_record.h>
#include <type_str.h>
#include <type_var.h>

#include <context.h>

value type_form(value fun, value arg)
	{
	return type_void(fun,arg);
	}

void put_error_location(unsigned long line, const char *label)
	{
	fprintf(stderr," on line %lu", line);
	if (label[0])
		fprintf(stderr," of %s\n", label);
	else
		fprintf(stderr,"\n");
	}

static void undefined_symbol(const char *name, unsigned long line,
	const char *label)
	{
	fprintf(stderr,"Undefined symbol %s",name);
	put_error_location(line,label);
	}

static value do_restrict(value cx, value form)
	{
	value label = form->L;
	value exp = form->R;
	value map = exp->L;
	value body = hold(exp->R);
	int has_undef = 0;

	while (map->T == 0)
		{
		if (cx->L == 0)
			{
			value sym = map->L->L->L;
			unsigned long line = map->L->L->R->v_double;
			undefined_symbol(str_data(sym),line,str_data(label));
			has_undef = 1;
			map = map->R;
			}
		else
			{
			value key = cx->L->L;
			value sym = map->L->L->L;
			int cmp = str_cmp(key->v_ptr,sym->v_ptr);

			if (cmp < 0)
				cx = cx->R;
			else if (cmp == 0)
				{
				value val = cx->L->R;
				value pattern = map->L->R;
				value new = subst(pattern,body,val);
				drop(body);
				body = new;
				cx = cx->R;
				map = map->R;
				}
			else
				{
				unsigned long line = map->L->L->R->v_double;
				undefined_symbol(str_data(sym),line,str_data(label));
				has_undef = 1;
				map = map->R;
				}
			}
		}
	if (has_undef)
		die(0);

	drop(form);
	return body;
	}

value type_restrict(value fun, value arg)
	{
	if (fun->L == 0)
		return need(fun,arg,type_record);
	else
		{
		arg = eval(arg);
		if (arg->T == type_form)
			{
			value exp = do_restrict(fun->R, arg);
			drop(fun);
			return exp;
			}
		else
			return type_void(fun,arg);
		}
	}

static value cx_std; // Current context

static void define(const char *key, value val)
	{
	// TODO use chain on the individual contexts
	cx_std = def(Qstr0(key),val,cx_std);
	}

// Define initial context.

static void use_basic(void)
	{
	define("I", hold(QI));
	define("T", hold(QT));
	define("F", hold(QF));
	define("@", Q(type_Y));
	define("void", hold(Qvoid));
	define("null", hold(Qnull));
	define("yield", Q(type_yield));
	define("cons", Q(type_cons));
	define("list_to_tuple", Q(type_list_to_tuple));
	define("tuple_to_list", Q(type_tuple_to_list));
	}

static void use_num(void)
	{
	define("num_str", Q(type_num_str));
	}

static void use_str(void)
	{
	define(".", Q(type_concat));

	// LATER length
	// LATER slice
	// LATER search
	// LATER str_num
	// LATER ord
	// LATER chr
	// LATER char_width
	// LATER dirname
	// LATER basename
	// LATER length_common
	// LATER compare_at
	}

static void use_math(void)
	{
	define("pi", Qnum(num_pi));

	define("round", Q(type_round));
	define("ceil", Q(type_ceil));
	define("trunc", Q(type_trunc));
	define("abs", Q(type_abs));
	define("sqrt", Q(type_sqrt));
	define("exp", Q(type_exp));
	define("log", Q(type_log));
	define("sin", Q(type_sin));
	define("cos", Q(type_cos));

	define("+", Q(type_add));
	define("-", Q(type_sub));
	define("*", Q(type_mul));
	define("/", Q(type_div));
	define("^", Q(type_pow));
	define("xor", Q(type_xor));
	}

static void use_output(void)
	{
	define("nl", A(Q(type_nl), hold(QI)));
	define("say", Q(type_say));
	define("put", Q(type_put));
	// LATER fput fsay
	}

static void use_meta(void)
	{
	// LATER introspection functions
	// LATER die
	define("show_benchmark", Q(type_show_benchmark));
	define("trace_benchmark", Q(type_trace_benchmark));
	define("show", Q(type_show));
	define("read", Q(type_read));
	}

static void use_limit(void)
	{
	define("limit_time", Q(type_limit_time));
	define("limit_stack", Q(type_limit_stack));
	define("limit_memory", Q(type_limit_memory));
	}

static void use_record(void)
	{
	define("empty", Q(type_record));
	define("def", Q(type_def));
	define("set", Q(type_set));
	define("get", Q(type_get));
	define("::", Q(type_chain));
	define("record_pairs", Q(type_record_pairs));
	}

static void use_var(void)
	{
	define("var_new", A(Q(type_var_new),hold(QI)));
	define("var_put", Q(type_var_put));
	define("var_get", Q(type_var_get));
	}

value std(void)
	{
	cx_std = Q(type_record); // empty stack

	use_basic();
	use_num();
	use_str();
	use_math();
	use_output();
	use_meta();
	use_limit();
	use_var();
	use_record();

	define("restrict",Q(type_restrict));
	define("std",hold(cx_std));

	// LATER time
	// LATER rand
	// LATER crypto
	return cx_std;
	}
