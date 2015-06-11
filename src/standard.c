#include <value.h>
#include <basic.h>
#include <num.h>
#include <standard.h>
#include <stdio.h>
#include <str.h>
#include <string.h> /* strcmp */
#include <type_buf.h>
#include <type_cmp.h>
#include <type_file.h>
#include <type_input.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_parse_file.h>
#include <type_parse_string.h>
#include <type_rand.h>
#include <type_run.h>
#include <type_str.h>
#include <type_var.h>

static const char *cur_name;

static int match(const char *other)
	{
	return strcmp(cur_name,other) == 0;
	}

static value type_standard(value f);

/* The standard (built-in) context */
static value standard(void)
	{
	if (match("^")) return Q(type_pow);
	if (match("-")) return Q(type_sub);
	if (match("/")) return Q(type_div);
	if (match(".")) return Q(type_concat);
	if (match("@")) return Q(type_Y);
	if (match("*")) return Q(type_mul);
	if (match("+")) return Q(type_add);
	if (match("abs")) return Q(type_abs);
	if (match("argv")) return Q(type_argv);
	if (match("buf_get")) return Q(type_buf_get);
	if (match("buf_new")) return Q(type_buf_new);
	if (match("buf_put")) return Q(type_buf_put);
	if (match("char_width")) return Q(type_char_width);
	if (match("cons")) return Q(type_cons);
	if (match("cos")) return Q(type_cos);
	if (match("die")) return Q(type_die);
	if (match("eq")) return Q(type_eq);
	if (match("exp")) return Q(type_exp);
	if (match("F")) return Q(type_F);
	if (match("fflush")) return Q(type_fflush);
	if (match("fget")) return Q(type_fget);
	if (match("flock_ex")) return Q(type_flock_ex);
	if (match("flock_sh")) return Q(type_flock_sh);
	if (match("flock_un")) return Q(type_flock_un);
	if (match("fopen")) return Q(type_fopen);
	if (match("fput")) return Q(type_fput);
	if (match("ge")) return Q(type_ge);
	if (match("get")) return Q(type_get);
	if (match("gt")) return Q(type_gt);
	if (match("I")) return Q(type_I);
	if (match("is_bool")) return Q(type_is_bool);
	if (match("is_good")) return Q(type_is_good);
	if (match("is_list")) return Q(type_is_list);
	if (match("is_num")) return Q(type_is_num);
	if (match("is_str")) return Q(type_is_str);
	if (match("is_void")) return Q(type_is_void);
	if (match("le")) return Q(type_le);
	if (match("length")) return Q(type_length);
	if (match("limit_memory")) return Q(type_limit_memory);
	if (match("limit_stack")) return Q(type_limit_stack);
	if (match("limit_time")) return Q(type_limit_time);
	if (match("log")) return Q(type_log);
	if (match("lt")) return Q(type_lt);
	if (match("ne")) return Q(type_ne);
	if (match("nl")) return Q(type_nl);
	if (match("null")) return Q(type_null);
	if (match("num_str")) return Q(type_num_str);
	if (match("once")) return Q(type_once);
	if (match("parse_file")) return Q(type_parse_file);
	if (match("parse_string")) return Q(type_parse_string);
	if (match("put")) return Q(type_put);
	if (match("put_to_error")) return Q(type_put_to_error);
	if (match("rand")) return Q(type_rand);
	if (match("remove")) return Q(type_remove);
	if (match("round")) return Q(type_round);
	if (match("say")) return Q(type_say);
	if (match("search")) return Q(type_search);
	if (match("seed_rand")) return Q(type_seed_rand);
	if (match("sin")) return Q(type_sin);
	if (match("slice")) return Q(type_slice);
	if (match("sqrt")) return Q(type_sqrt);
	if (match("standard")) return Q(type_standard);
	if (match("stdin")) return Qfile(stdin);
	if (match("stderr")) return Qfile(stderr);
	if (match("stdout")) return Qfile(stdout);
	if (match("str_num")) return Q(type_str_num);
	if (match("T")) return Q(type_T);
	if (match("trunc")) return Q(type_trunc);
	if (match("var_get")) return Q(type_var_get);
	if (match("var_new")) return Q(type_var_new);
	if (match("var_put")) return Q(type_var_put);
	if (match("void")) return Q(type_void);

	return 0;
	}

/* (standard x) = {def} if x is defined, or void if x is not defined. */
static value type_standard(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		value def;
		cur_name = ((string)data(x))->data;
		def = standard();
		if (def)
			replace_single(f, def);
		else
			replace_void(f);
		}
	else
		replace_void(f);
	drop(x);
	return 0;
	}
	}

/* Evaluate the named file in the standard context.  Use stdin if the name is
null or empty. */
value eval_file(const char *name)
	{
	value label = Qstr(str_new_data0(name ? name : ""));
	value exp = A(Q(type_parse_file),label);
	value context = Q(type_standard);
	return eval(A(exp,context));
	}
