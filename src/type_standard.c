#include <num.h>
#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <string.h> /* strcmp */
#include <type_buf.h>
#include <type_cmp.h>
#include <type_file.h>
#include <type_istr.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_parse.h>
#include <type_rand.h>
#include <type_resolve.h>
#include <type_run.h>
#include <type_standard.h>
#include <type_str.h>
#include <type_var.h>

#if DEV
#include <file.h>
#include <memory.h>
#endif

static value Qresolve;
static value Qstandard;

static const char *cur_name;

static int match(const char *other)
	{
	return strcmp(cur_name,other) == 0;
	}

/* The standard (built-in) context */
static value standard(void)
	{
	if (match("put")) return hold(Qput);
	if (match("nl")) return hold(Qnl);
	if (match("say")) return Q(type_say);
	if (match("fput")) return hold(Qfput);
	if (match("fnl")) return hold(Qfnl);
	if (match("fsay")) return Q(type_fsay);
	if (match("fflush")) return Q(type_fflush);

	if (match("+")) return Q(type_add);
	if (match("-")) return Q(type_sub);
	if (match("*")) return Q(type_mul);
	if (match("/")) return Q(type_div);
	if (match("^")) return Q(type_pow);
	if (match("xor")) return Q(type_xor);
	if (match("round")) return Q(type_round);
	if (match("trunc")) return Q(type_trunc);
	if (match("abs")) return Q(type_abs);
	if (match("sqrt")) return Q(type_sqrt);
	if (match("exp")) return Q(type_exp);
	if (match("log")) return Q(type_log);
	if (match("sin")) return Q(type_sin);
	if (match("cos")) return Q(type_cos);
	if (match("pi")) return Qnum(num_pi());

	if (match("lt")) return Q(type_lt);
	if (match("le")) return Q(type_le);
	if (match("eq")) return Q(type_eq);
	if (match("ne")) return Q(type_ne);
	if (match("ge")) return Q(type_ge);
	if (match("gt")) return Q(type_gt);

	if (match("I")) return hold(QI);
	if (match("T")) return hold(QT);
	if (match("F")) return hold(QF);
	if (match("@")) return hold(QY);
	if (match("void")) return hold(Qvoid);
	if (match("cons")) return hold(Qcons);
	if (match("null")) return hold(Qnull);
	if (match("once")) return Q(type_once);
	if (match("later")) return Q(type_later);
	if (match("is_void")) return Q(type_is_void);
	if (match("is_good")) return Q(type_is_good);
	if (match("is_bool")) return Q(type_is_bool);
	if (match("is_list")) return Q(type_is_list);

	if (match(".")) return Q(type_concat);
	if (match("length")) return Q(type_length);
	if (match("slice")) return Q(type_slice);
	if (match("search")) return Q(type_search);
	if (match("str_num")) return Q(type_str_num);
	if (match("ord")) return Q(type_ord);
	if (match("chr")) return Q(type_chr);
	if (match("char_width")) return Q(type_char_width);
	if (match("is_str")) return Q(type_is_str);

	if (match("num_str")) return Q(type_num_str);
	if (match("is_num")) return Q(type_is_num);

	if (match("stdin")) return Qfile(stdin);
	if (match("stdout")) return Qfile(stdout);
	if (match("stderr")) return Qfile(stderr);
	if (match("fopen")) return Q(type_fopen);
	if (match("fclose")) return Q(type_fclose);
	if (match("fgetc")) return Q(type_fgetc);
	if (match("fget")) return Q(type_fget);
	if (match("remove")) return Q(type_remove);
	if (match("flock_ex")) return Q(type_flock_ex);
	if (match("flock_sh")) return Q(type_flock_sh);
	if (match("flock_un")) return Q(type_flock_un);

	if (match("die")) return Q(type_die);
	if (match("argv")) return Q(type_argv);

	if (match("seed_rand")) return Q(type_seed_rand);
	if (match("rand")) return Q(type_rand);

	if (match("standard")) return hold(Qstandard);
	if (match("parse")) return Q(type_parse);
	if (match("resolve")) return hold(Qresolve);
	if (match("use")) return Q(type_use);

	if (match("buf_new")) return Q(type_buf_new);
	if (match("buf_put")) return Q(type_buf_put);
	if (match("buf_get")) return Q(type_buf_get);

	if (match("readstr")) return Q(type_readstr);
	if (match("sgetc")) return Q(type_sgetc);
	if (match("sget")) return Q(type_sget);

	if (match("var_new")) return Q(type_var_new);
	if (match("var_get")) return Q(type_var_get);
	if (match("var_put")) return Q(type_var_put);

	if (match("limit_time")) return Q(type_limit_time);
	if (match("limit_memory")) return Q(type_limit_memory);
	if (match("limit_stack")) return Q(type_limit_stack);

	return 0;
	}

/* (standard sym) returns {val} if the symbol is defined as val, or void if the
symbol is not defined.  It returns val as a singleton so it's not evaluated. */
value type_standard(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		value def;
		cur_name = str_data(x);
		def = standard();
		if (def)
			f = A(hold(Qyield),def);
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* Return a function which evaluates the expression in the given context. */
static value use_context(value context, value exp)
	{
	return eval(A(A(hold(Qresolve),context),exp));
	}

/* Return a function which evaluates the named file in the standard context. */
static value parse_standard(value name)
	{
	return use_context(hold(Qstandard),parse_file(name));
	}

/* (use file exp)
Equivalent to:
	(
	\context=(parse_file file standard)
	\f=(resolve context exp)
	f
	)
This is used to bootstrap new contexts written in Fexl so you can do this:
	use "lib.fxl" \; ...
*/
value type_use(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value name = arg(f->L->R);
	if (name->T == type_str)
		{
		value context = parse_standard(hold(name));
		value exp = hold(f->R);
		f = use_context(context,exp);
		}
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}
	}

static void beg_standard(void)
	{
	beg_basic();
	beg_output();
	Qresolve = Q(type_resolve);
	Qstandard = Q(type_standard);
	}

static void end_standard(void)
	{
	end_basic();
	end_output();
	drop(Qresolve);
	drop(Qstandard);
	end_value();
	}

/* Evaluate a named file in the standard context. */
void eval_file(const char *name)
	{
	beg_standard();
	drop(eval(parse_standard(Qstr0(name))));

	#if DEV
	if (0)
	{
	fnl(stderr);
	fput(stderr,"num_steps = ");fput_ulong(stderr,num_steps);fnl(stderr);
	fput(stderr,"max_bytes = ");fput_ulong(stderr,max_bytes);fnl(stderr);
	fput(stderr,"count_V = ");fput_ulong(stderr,count_V);fnl(stderr);
	}
	#endif

	end_standard();
	}
