#include <num.h>
#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <fexl.h>
#include <output.h>
#include <output2.h>
#include <show.h>
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
#include <type_resolve.h>
#include <type_run.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_var.h>

void put_type(type t)
	{
	if (t == type_A) put_ch('A');

	else if (t == type_sym) put("sym");
	else if (t == type_subst) put("subst");

	else if (t == type_form) put("form");

	else if (t == type_put) put("put");
	else if (t == type_nl) put("nl");
	else if (t == type_say) put("say");
	else if (t == type_fput) put("fput");
	else if (t == type_fnl) put("fnl");
	else if (t == type_fsay) put("fsay");
	else if (t == type_fflush) put("fflush");

	else if (t == type_add) put("add");
	else if (t == type_sub) put("sub");
	else if (t == type_mul) put("mul");
	else if (t == type_div) put("div");
	else if (t == type_pow) put("pow");
	else if (t == type_xor) put("xor");
	else if (t == type_round) put("round");
	else if (t == type_trunc) put("trunc");
	else if (t == type_abs) put("abs");
	else if (t == type_sqrt) put("sqrt");
	else if (t == type_exp) put("exp");
	else if (t == type_log) put("log");
	else if (t == type_sin) put("sin");
	else if (t == type_cos) put("cos");

	else if (t == type_lt) put("lt");
	else if (t == type_le) put("le");
	else if (t == type_eq) put("eq");
	else if (t == type_ne) put("ne");
	else if (t == type_ge) put("ge");
	else if (t == type_gt) put("gt");

	else if (t == type_I) put_ch('I');
	else if (t == type_T) put_ch('T');
	else if (t == type_F) put_ch('F');
	else if (t == type_Y) put_ch('@');
	else if (t == type_void) put("void");
	else if (t == type_cons) put("cons");
	else if (t == type_null) put("null");
	else if (t == type_yield) put("yield");
	else if (t == type_eval) put("eval");
	else if (t == type_O) put_ch('O');
	else if (t == type_once) put("once");
	else if (t == type_later) put("later");
	else if (t == type_is_void) put("is_void");
	else if (t == type_is_good) put("is_good");
	else if (t == type_is_bool) put("is_bool");
	else if (t == type_is_list) put("is_list");

	else if (t == type_str) put("str");
	else if (t == type_concat) put("concat");
	else if (t == type_length) put("length");
	else if (t == type_slice) put("slice");
	else if (t == type_search) put("search");
	else if (t == type_str_num) put("str_num");
	else if (t == type_ord) put("ord");
	else if (t == type_chr) put("chr");
	else if (t == type_char_width) put("char_width");
	else if (t == type_is_str) put("is_str");

	else if (t == type_num) put("num");
	else if (t == type_num_str) put("num_str");
	else if (t == type_is_num) put("is_num");

	else if (t == type_file) put("file");
	else if (t == type_fopen) put("fopen");
	else if (t == type_fclose) put("fclose");
	else if (t == type_fgetc) put("fgetc");
	else if (t == type_fget) put("fget");
	else if (t == type_remove) put("remove");
	else if (t == type_flock_ex) put("flock_ex");
	else if (t == type_flock_sh) put("flock_sh");
	else if (t == type_flock_un) put("flock_un");

	else if (t == type_die) put("die");
	else if (t == type_argv) put("argv");

	else if (t == type_seed_rand) put("seed_rand");
	else if (t == type_rand) put("rand");

	else if (t == type_standard) put("standard");
	else if (t == type_use) put("use");

	else if (t == type_parse) put("parse");

	else if (t == type_resolve) put("resolve");

	else if (t == type_buf) put("buf");
	else if (t == type_buf_new) put("buf_new");
	else if (t == type_buf_put) put("buf_put");
	else if (t == type_buf_get) put("buf_get");

	else if (t == type_istr) put("istr");
	else if (t == type_readstr) put("readstr");
	else if (t == type_sgetc) put("sgetc");
	else if (t == type_sget) put("sget");

	else if (t == type_var) put("var");
	else if (t == type_var_new) put("var_new");
	else if (t == type_var_get) put("var_get");
	else if (t == type_var_put) put("var_put");

	else if (t == type_limit_time) put("limit_time");
	else if (t == type_limit_stack) put("limit_stack");
	else if (t == type_limit_memory) put("limit_memory");

	else put_ch('?');
	}

static void put_quote(string x)
	{
	put_ch('"');
	put_str(x);
	put_ch('"');
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
	put_type(f->T);
	if (f->L)
		{
		put_ch(' ');
		limit_show(f->L);
		put_ch(' ');
		limit_show(f->R);
		}
	else if (f->R)
		{
		put_ch(' ');
		if (f->T == type_num)
			put_num(get_num(f));
		else if (f->T == type_str)
			put_quote(get_str(f));
		else if (f->T == type_sym)
			{
			put_quote(sym_name(f));
			if (0)
			{
			put_ch(' ');
			put_ulong(sym_line(f));
			}
			}
		else if (f->T == type_form)
			{
			put_quote(get_str(form_label(f)));
			put_ch(' ');
			limit_show(form_exp(f));
			}
		else if (f->T == type_var)
			limit_show(f->R);
		else if (f->T == type_buf || f->T == type_istr)
			put("...");
		else if (f->T == type_file)
			put_ulong(fileno(get_fh(f)));
		else
			put_ch('?');
		}
	put_ch(']');

	max_depth++;
	}

void show(value f)
	{
	max_depth = 12;
	max_call = 200;
	limit_show(f);
	}

void show_line(const char *name, value f)
	{
	put(name);show(f);nl();
	}
