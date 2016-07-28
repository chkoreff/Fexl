#include <stdio.h>
#include <value.h>
#include <basic.h>
#include <fexl.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <test/show.h>
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
#include <type_str.h>
#include <type_sym.h>
#include <type_var.h>

void put(const char *data)
	{
	fput(stdout,data);
	}

void put_ch(char ch)
	{
	fput_ch(stdout,ch);
	}

void put_ulong(unsigned long x)
	{
	fput_ulong(stdout,x);
	}

void put_str(string x)
	{
	fput_str(stdout,x);
	}

void put_num(number x)
	{
	fput_num(stdout,x);
	}

void put_type(type t)
	{
	if (0) ;
	else if (t == type_A) put("A");
	else if (t == type_add) put("add");
	else if (t == type_abs) put("abs");
	else if (t == type_argv) put("argv");
	else if (t == type_buf) put("buf");
	else if (t == type_buf_get) put("buf_get");
	else if (t == type_buf_new) put("buf_new");
	else if (t == type_buf_put) put("buf_put");
	else if (t == type_char_width) put("char_width");
	else if (t == type_chr) put("chr");
	else if (t == type_concat) put("concat");
	else if (t == type_die) put("die");
	else if (t == type_div) put("div");
	else if (t == type_cons) put("cons");
	else if (t == type_cos) put("cos");
	else if (t == type_eq) put("eq");
	else if (t == type_eval) put("eval");
	else if (t == type_exp) put("exp");
	else if (t == type_F) put_ch('F');
	else if (t == type_fflush) put("fflush");
	else if (t == type_fget) put("fget");
	else if (t == type_fgetc) put("fgetc");
	else if (t == type_file) put("file");
	else if (t == type_flock_ex) put("flock_ex");
	else if (t == type_flock_sh) put("flock_sh");
	else if (t == type_flock_un) put("flock_un");
	else if (t == type_fopen) put("fopen");
	else if (t == type_fput) put("fput");
	else if (t == type_fsay) put("fsay");
	else if (t == type_ge) put("ge");
	else if (t == type_gt) put("gt");
	else if (t == type_I) put_ch('I');
	else if (t == type_is_bool) put("is_bool");
	else if (t == type_is_good) put("is_good");
	else if (t == type_is_list) put("is_list");
	else if (t == type_is_num) put("is_num");
	else if (t == type_is_str) put("is_str");
	else if (t == type_is_void) put("is_void");
	else if (t == type_istr) put("istr");
	else if (t == type_le) put("le");
	else if (t == type_length) put("length");
	else if (t == type_limit_memory) put("limit_memory");
	else if (t == type_limit_time) put("limit_time");
	else if (t == type_limit_stack) put("limit_stack");
	else if (t == type_log) put("log");
	else if (t == type_lt) put("lt");
	else if (t == type_mul) put("mul");
	else if (t == type_ne) put("ne");
	else if (t == type_nl) put("nl");
	else if (t == type_null) put("null");
	else if (t == type_num) put("num");
	else if (t == type_num_str) put("num_str");
	else if (t == type_once) put("once");
	else if (t == type_ord) put("ord");
	else if (t == type_parse) put("parse");
	else if (t == type_pow) put("pow");
	else if (t == type_put) put("put");
	else if (t == type_rand) put("rand");
	else if (t == type_readstr) put("readstr");
	else if (t == type_remove) put("remove");
	else if (t == type_resolve) put("resolve");
	else if (t == type_round) put("round");
	else if (t == type_say) put("say");
	else if (t == type_search) put("search");
	else if (t == type_seed_rand) put("seed_rand");
	else if (t == type_sget) put("sget");
	else if (t == type_sgetc) put("sgetc");
	else if (t == type_sin) put("sin");
	else if (t == type_slice) put("slice");
	else if (t == type_sqrt) put("sqrt");
	else if (t == type_standard) put("standard");
	else if (t == type_str) put("str");
	else if (t == type_str_num) put("str_num");
	else if (t == type_sub) put("sub");
	else if (t == type_subst) put("subst");
	else if (t == type_sym) put("sym");
	else if (t == type_T) put_ch('T');
	else if (t == type_trunc) put("trunc");
	else if (t == type_use) put("use");
	else if (t == type_var) put("var");
	else if (t == type_var_get) put("var_get");
	else if (t == type_var_new) put("var_new");
	else if (t == type_var_put) put("var_put");
	else if (t == type_void) put("void");
	else if (t == type_xor) put("xor");
	else if (t == type_Y) put_ch('@');
	else if (t == type_yield) put("yield");
	else
		{
		put_ch('T');
		put_ulong((unsigned long)t);
		}
	}

void show(value f)
	{
	put("[");
	if (0)
	{
	put_ulong(f->N);put_ch(' ');
	}
	put_type(f->T);
	if (f->L)
		{
		put_ch(' ');
		show(f->L);
		put_ch(' ');
		show(f->R);
		}
	else if (f->R)
		{
		put_ch(' ');
		if (f->T == type_num)
			put_num(data(f));
		else if (f->T == type_str)
			{
			put_ch('"');
			put_str(data(f));
			put_ch('"');
			}
		else if (f->T == type_sym)
			{
			symbol sym = data(f);
			put_ch('"');
			put_str(data(sym->name));
			put_ch('"');
			}
		else if (f->T == type_var)
			show(data(f));
		else if (f->T == type_buf)
			put("...");
		else if (f->T == type_file)
			{
			FILE *fh = data(f);
			put_ulong(fileno(fh));
			}
		else
			put("?");
		}
	put("]");
	}
