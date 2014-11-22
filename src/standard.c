#include <value.h>
#include <basic.h>
#include <die.h>
#include <file_input.h>
#include <input.h>
#include <num.h>
#include <output.h>
#include <parse.h>
#include <source.h>
#include <standard.h>
#include <str.h>
#include <str_input.h>
#include <string.h> /* strcmp */
#include <type_cmp.h>
#include <type_input.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_rand.h>
#include <type_str.h>
#include <type_sym.h>

/*LATER A "cd" function which temporarily changes directory, then back. */

static const char *cur_name;

static int match(const char *other)
	{
	return strcmp(cur_name,other) == 0;
	}

static value type_standard(value f);
static value type_eval_file(value f);
static value type_eval_str(value f);

static value type_die(value f)
	{
	(void)f;
	die(0);
	return 0;
	}

static value standard_name(const char *name)
	{
	{
	value def = Qnum_str0(name);
	if (def) return def;
	}

	cur_name = name;

	if (match("^")) return Q(type_pow);
	if (match("-")) return Q(type_sub);
	if (match("/")) return Q(type_div);
	if (match(".")) return Q(type_concat);
	if (match("@")) return hold(Y);
	if (match("*")) return Q(type_mul);
	if (match("+")) return Q(type_add);
	if (match("?")) return Q(type_query);
	if (match("abs")) return Q(type_abs);
	if (match("void")) return Q(type_void);
	if (match("cons")) return hold(cons);
	if (match("cos")) return Q(type_cos);
	if (match("die")) return Q(type_die);
	if (match("eq")) return Q(type_eq);
	if (match("eval_file")) return Q(type_eval_file);
	if (match("eval_str")) return Q(type_eval_str);
	if (match("exp")) return Q(type_exp);
	if (match("F")) return Q(type_F);
	if (match("ge")) return Q(type_ge);
	if (match("get")) return Q(type_get);
	if (match("gt")) return Q(type_gt);
	if (match("I")) return hold(I);
	if (match("is_bool")) return Q(type_is_bool);
	if (match("is_good")) return Q(type_is_good);
	if (match("is_list")) return Q(type_is_list);
	if (match("is_num")) return Q(type_is_num);
	if (match("is_str")) return Q(type_is_str);
	if (match("is_void")) return Q(type_is_void);
	if (match("le")) return Q(type_le);
	if (match("length")) return Q(type_length);
	if (match("log")) return Q(type_log);
	if (match("lt")) return Q(type_lt);
	if (match("ne")) return Q(type_ne);
	if (match("nl")) return Q(type_nl);
	if (match("null")) return hold(null);
	if (match("num_str")) return Q(type_num_str);
	if (match("once")) return Q(type_once);
	if (match("put")) return Q(type_put);
	if (match("put_to_error")) return Q(type_put_to_error);
	if (match("rand")) return Q(type_rand);
	if (match("round")) return Q(type_round);
	if (match("say")) return Q(type_say);
	if (match("seed_rand")) return Q(type_seed_rand);
	if (match("sin")) return Q(type_sin);
	if (match("slice")) return Q(type_slice);
	if (match("sqrt")) return Q(type_sqrt);
	if (match("standard")) return Q(type_standard);
	if (match("str_num")) return Q(type_str_num);
	if (match("T")) return Q(type_T);
	if (match("trunc")) return Q(type_trunc);

	return 0;
	}

static value standard_context(value x)
	{
	const char *name = ((string)x->R->R)->data;
	return standard_name(name);
	}

static value type_standard(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		value def = standard_context(x);
		if (def)
			replace_A(f, A(hold(cons),def), hold(null));
		else
			{
			replace_Q(f, type_null);
			f = 0;
			}
		}
	else
		{
		replace_void(f);
		f = 0;
		}
	drop(x);
	return f;
	}
	}

static value parse_standard(const char *label)
	{
	value exp;
	const char *save = source_label;
	source_label = label;
	exp = resolve(parse_source(),standard_context);
	source_label = save;
	return exp;
	}

value eval_file(const char *name)
	{
	value exp;
	input save = getd;

	get_from_file(name);
	if (!getd)
		{
		put_to_error();
		put("Could not open source file ");put(name);nl();
		die(0);
		}
	exp = parse_standard(name);

	getd = save;
	return eval(exp);
	}

static value eval_str(string x)
	{
	value exp;
	input save = getd;

	get_from_string(x);
	exp = parse_standard(0);

	getd = save;
	return eval(exp);
	}

static value type_eval_file(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		const char *name = ((string)x->R->R)->data;
		value g = eval_file(name);
		drop(f);
		f = g;
		}
	else
		{
		replace_void(f);
		f = 0;
		}
	drop(x);
	return f;
	}
	}

static value type_eval_str(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		value g = eval_str((string)x->R->R);
		drop(f);
		f = g;
		}
	else
		{
		replace_void(f);
		f = 0;
		}
	drop(x);
	return f;
	}
	}

/*
# LATER get_from_file
# LATER get_from_string
# LATER get_from_input
# LATER get_from_source
# LATER parse
*/

/*LATER eval any input stream (file, string, etc.) */
/*LATER use arbitrary context, not just the standard one */

/*LATER after we do parse in fexl, we'll no longer need eval_file
and eval_str defined in C.  We can define those in fexl easily.

\eval_file=
	(\name
	get_from_file name ;
	\source_name=name
	parse source_name
	)

\eval_str=
	(\text
	get_from_string text ;
	\source_name=""
	parse source_name
	)
*/

