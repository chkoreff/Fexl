#include <num.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <convert.h>
#include <file_input.h>
#include <input.h>
#include <parse.h>
#include <source.h>
#include <standard.h>
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

static const char *curr_name;

static int match(const char *other)
	{
	return strcmp(curr_name,other) == 0;
	}

static value type_eval_file(value f);
static value type_eval_str(value f);

static value standard_name(const char *name)
	{
	{
	value def = Qnum_str0(name);
	if (def) return def;
	}

	curr_name = name;

	if (match("^")) return Q(type_pow);
	if (match("-")) return Q(type_sub);
	if (match("/")) return Q(type_div);
	if (match(".")) return Q(type_concat);
	if (match("@")) return hold(Y);
	if (match("*")) return Q(type_mul);
	if (match("+")) return Q(type_add);
	if (match("abs")) return Q(type_abs);
	if (match("void")) return Q(type_void);
	if (match("cons")) return hold(Qcons);
	if (match("cos")) return Q(type_cos);
	if (match("eq")) return Q(type_eq);
	if (match("eval_file")) return Q(type_eval_file);
	if (match("eval_str")) return Q(type_eval_str);
	if (match("F")) return hold(F);
	if (match("ge")) return Q(type_ge);
	if (match("get")) return Q(type_get);
	if (match("gt")) return Q(type_gt);
	if (match("I")) return hold(I);
	if (match("is_void")) return Q(type_is_void);
	if (match("is_num")) return Q(type_is_num);
	if (match("is_str")) return Q(type_is_str);
	if (match("is_bool")) return Q(type_is_bool);
	if (match("later")) return Q(type_later);
	if (match("le")) return Q(type_le);
	if (match("length")) return Q(type_length);
	if (match("log")) return Q(type_log);
	if (match("lt")) return Q(type_lt);
	if (match("ne")) return Q(type_ne);
	if (match("nl")) return Q(type_nl);
	if (match("null")) return hold(C);
	if (match("num_str")) return Q(type_num_str);
	if (match("put")) return Q(type_put);
	if (match("put_to_error")) return Q(type_put_to_error);
	if (match("rand")) return Q(type_rand);
	if (match("round")) return Q(type_round);
	if (match("say")) return Q(type_say);
	if (match("seed_rand")) return Q(type_seed_rand);
	if (match("sin")) return Q(type_sin);
	if (match("slice")) return Q(type_slice);
	if (match("sqrt")) return Q(type_sqrt);
	if (match("str_num")) return Q(type_str_num);
	if (match("T")) return hold(T);
	if (match("trunc")) return Q(type_trunc);

	return 0;
	}

value standard_context(value x)
	{
	const char *name = ((string)x->R)->data;
	return standard_name(name);
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
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		const char *name = ((string)x->R)->data;
		f = eval_file(name);
		}
	else
		f = Q(type_void);
	drop(x);
	return f;
	}
	}

static value type_eval_str(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		f = eval_str((string)x->R);
	else
		f = Q(type_void);
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

