#include <num.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <standard.h>
#include <stdio.h>
#include <string.h> /* strcmp */
#include <type_buf.h>
#include <type_cmp.h>
#include <type_file.h>
#include <type_form.h>
#include <type_istr.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_parse.h>
#include <type_rand.h>
#include <type_run.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_tuple.h>
#include <type_var.h>

static value QY;
static value Quse_standard;
static value Qparse_file;
static value Qevaluate;

static const char *cur_name;

static int match(const char *other)
	{
	return strcmp(cur_name,other) == 0;
	}

/* Resolve standard symbols. */
static value resolve_standard(const char *name)
	{
	{
	/* Resolve numeric literals. */
	value def = Qnum_str0(name);
	if (def) return def;
	}

	/* Resolve other names. */
	cur_name = name;

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
	if (match("later")) return hold(Qlater);
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
	if (match("dirname")) return Q(type_dirname);
	if (match("basename")) return Q(type_basename);
	if (match("is_str")) return Q(type_is_str);

	if (match("num_str")) return Q(type_num_str);
	if (match("is_num")) return Q(type_is_num);

	if (match("is_tuple")) return Q(type_is_tuple);
	if (match("arity")) return Q(type_arity);
	if (match("split_tuple")) return Q(type_split_tuple);
	if (match("join_tuple")) return Q(type_join_tuple);

	if (match("stdin")) return Qfile(stdin);
	if (match("stdout")) return Qfile(stdout);
	if (match("stderr")) return Qfile(stderr);
	if (match("fopen")) return Q(type_fopen);
	if (match("fclose")) return Q(type_fclose);
	if (match("fgetc")) return Q(type_fgetc);
	if (match("fget")) return Q(type_fget);
	if (match("remove")) return Q(type_remove);
	if (match("is_newer")) return Q(type_is_newer);
	if (match("flock_ex")) return Q(type_flock_ex);
	if (match("flock_sh")) return Q(type_flock_sh);
	if (match("flock_un")) return Q(type_flock_un);
	if (match("readlink")) return Q(type_readlink);

	if (match("die")) return Q(type_die);
	if (match("argv")) return Q(type_argv);

	if (match("seed_rand")) return Q(type_seed_rand);
	if (match("rand")) return Q(type_rand);

	if (match("use_standard")) return hold(Quse_standard);
	if (match("use_numbers")) return Q(type_use_numbers);

	if (match("evaluate")) return hold(Qevaluate);
	if (match("evaluate_later")) return Q(type_evaluate_later);
	if (match("is_resolved")) return Q(type_is_resolved);
	if (match("define")) return Q(type_define);

	if (match("parse")) return Q(type_parse);
	if (match("parse_file")) return hold(Qparse_file);

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

/* (use_standard form) Resolve standard symbols in the form. */
value type_use_standard(value f)
	{
	return op_resolve(resolve_standard,f);
	}

/* (use_numbers form) Resolve numeric literals in the form. */
value type_use_numbers(value f)
	{
	return op_resolve(Qnum_str0,f);
	}

value QI;
value QT;
value QF;
value Qvoid;
value Qcons;
value Qnull;
value Qeval;
value Qlater;
value Qput;
value Qnl;
value Qfput;
value Qfnl;
value Qtuple;

static void beg_const(void)
	{
	QI = Q(type_I);
	QT = Q(type_T);
	QF = Q(type_F);
	QY = Q(type_Y);
	Qvoid = Q(type_void);
	Qcons = Q(type_cons);
	Qnull = Q(type_null);
	Qeval = Q(type_eval);
	Qlater = Q(type_later);
	Quse_standard = Q(type_use_standard);
	Qput = Q(type_put);
	Qnl = Q(type_nl);
	Qfput = Q(type_fput);
	Qfnl = Q(type_fnl);
	Qparse_file = Q(type_parse_file);
	Qevaluate = Q(type_evaluate);
	Qtuple = Q(type_tuple);
	}

static void end_const(void)
	{
	drop(QI);
	drop(QT);
	drop(QF);
	drop(QY);
	drop(Qvoid);
	drop(Qcons);
	drop(Qnull);
	drop(Qeval);
	drop(Qlater);
	drop(Quse_standard);
	drop(Qput);
	drop(Qnl);
	drop(Qfput);
	drop(Qfnl);
	drop(Qparse_file);
	drop(Qevaluate);
	drop(Qtuple);
	}

/* Evaluate main.fxl located relative to the executable given by argv[0].  The
main.fxl script then evaluates the user's script given by argv[1]. */
static value eval_script(void)
	{
	value f;
	/* Get argv[0]. */
	f = A(Q(type_argv),Qnum(num_new_ulong(0)));
	/* Go two directories up to get above the bin directory. */
	f = A(Q(type_dirname),f);
	f = A(Q(type_dirname),f);
	/* Concatenate the name of the main script. */
	f = A(A(Q(type_concat),f),Qstr0("/src/main.fxl"));
	/* Now evaluate the main script. */
	f = A(hold(Qparse_file),f);
	f = A(hold(Quse_standard),f);
	f = A(hold(Qevaluate),f);
	f = eval(f);
	return f;
	}

void eval_standard(void)
	{
	beg_const();
	drop(eval_script());
	end_const();
	end_value();
	}
