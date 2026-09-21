#include <str.h>
#include <value.h>

#include <basic.h>
#include <report.h>
#include <stdio.h>
#include <string.h> // strcmp strlen
#include <test.h>
#include <type_argv.h>
#include <type_bn.h>
#include <type_buf.h>
#include <type_cmp.h>
#include <type_crypto.h>
#include <type_fexl.h>
#include <type_file.h>
#include <type_hex.h>
#include <type_istr.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_parse.h>
#include <type_rand.h>
#include <type_record.h>
#include <type_run.h>
#include <type_signal.h>
#include <type_str.h>
#include <type_stream.h>
#include <type_sym.h>
#include <type_time.h>
#include <type_tuple.h>
#include <type_var.h>

//#include <memory.h>

static void beg_const(void)
	{
	beg_argv();
	beg_basic();
	beg_crypto();
	beg_file();
	beg_output();
	beg_signal();
	beg_sym();
	beg_tuple();
	}

static void end_const(void)
	{
	end_argv();
	end_basic();
	end_crypto();
	end_file();
	end_output();
	end_signal();
	end_sym();
	end_tuple();
	}

// Define all the functions written in C.
static void load_core(void)
	{
	load_argv();
	load_basic();
	load_bn();
	load_buf();
	load_cmp();
	load_crypto();
	load_fexl();
	load_file();
	load_hex();
	load_istr();
	load_limit();
	load_math();
	load_num();
	load_output();
	load_parse();
	load_rand();
	load_record();
	load_run();
	load_signal();
	load_str();
	load_stream();
	load_sym();
	load_time();
	load_tuple();
	load_var();
	}

static int eq(const char *x, const char *y)
	{
	return strcmp(x,y) == 0;
	}

// Load a named library.
static void load(const char *name)
	{
	if (eq(name,"argv")) return load_argv();
	if (eq(name,"assoc")) return use_lib("assoc.fxl");
	if (eq(name,"basic")) return load_basic();
	if (eq(name,"bn")) return load_bn();
	if (eq(name,"bool")) return use_lib("bool.fxl");
	if (eq(name,"buf")) return load_buf();
	if (eq(name,"cmp")) return load_cmp();
	if (eq(name,"core")) return load_core();
	if (eq(name,"crypto")) return load_crypto();
	if (eq(name,"date")) return use_lib("date.fxl");
	if (eq(name,"extra")) return use_lib("extra.fxl");
	if (eq(name,"fexl")) return load_fexl();
	if (eq(name,"file")) return load_file();
	if (eq(name,"format")) return use_lib("format.fxl");
	if (eq(name,"hex")) return load_hex();
	if (eq(name,"hex2")) return use_lib("hex.fxl");
	if (eq(name,"html")) return use_lib("html.fxl");
	if (eq(name,"indent")) return use_lib("indent.fxl");
	if (eq(name,"istr")) return load_istr();
	if (eq(name,"limit")) return load_limit();
	if (eq(name,"list")) return use_lib("list.fxl");
	if (eq(name,"main")) return use_lib("main.fxl");
	if (eq(name,"math")) return load_math();
	if (eq(name,"math2")) return use_lib("math.fxl");
	if (eq(name,"num")) return load_num();
	if (eq(name,"output")) return load_output();
	if (eq(name,"parse")) return load_parse();
	if (eq(name,"rand")) return load_rand();
	if (eq(name,"read")) return use_lib("read.fxl");
	if (eq(name,"read_csv")) return use_lib("read_csv.fxl");
	if (eq(name,"read_ssv")) return use_lib("read_ssv.fxl");
	if (eq(name,"record")) return load_record();
	if (eq(name,"run")) return load_run();
	if (eq(name,"run2")) return use_lib("run.fxl");
	if (eq(name,"show_value")) return use_lib("show_value.fxl");
	if (eq(name,"signal")) return load_signal();
	if (eq(name,"str")) return load_str();
	if (eq(name,"stream")) return load_stream();
	if (eq(name,"sym")) return load_sym();
	if (eq(name,"test")) return load_test();
	if (eq(name,"time")) return load_time();
	if (eq(name,"time2")) return use_lib("time.fxl");
	if (eq(name,"tuple")) return load_tuple();
	if (eq(name,"var")) return load_var();
	bad_name("Unknown library ",name);
	}

static value type_load(value f)
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		load(str_data(x));
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

/*
Evaluate the user's script.  Read the script from the file named by argv[1] if
present, or from stdin otherwise.  If the name designates a directory it
behaves like an empty file.

If this program is running as "fexl", it loads the "main" library.

If this program is running as "fexl0", it does not load any libraries.
*/
static void eval_script(void)
	{
	const char *path = main_argv[0];
	unsigned long len = strlen(path);
	char ch = len == 0 ? 0 : path[len-1];

	define("load",Q(type_load));
	define("evaluate",Q(type_evaluate));

	if (ch != '0') load("main");

	use_file(main_argc > 1 ? main_argv[1] : "");
	}

int main(int argc, const char *argv[])
	{
	main_argc = argc;
	main_argv = argv;
	beg_const();
	eval_script();
	//printf("END steps %lu bytes %lu\n",cur_steps,cur_bytes);
	end_const();
	end_value();
	return 0;
	}
