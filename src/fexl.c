#include <str.h>
#include <value.h>

#include <basic.h>
#include <define.h>
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
static void define_core(void)
	{
	define_argv();
	define_basic();
	define_bn();
	define_buf();
	define_cmp();
	define_crypto();
	define_fexl();
	define_file();
	define_hex();
	define_istr();
	define_limit();
	define_math();
	define_num();
	define_output();
	define_parse();
	define_rand();
	define_record();
	define_run();
	define_signal();
	define_str();
	define_stream();
	define_sym();
	define_time();
	define_tuple();
	define_use_test();
	define_var();
	}

static int eq(const char *x, const char *y)
	{
	return strcmp(x,y) == 0;
	}

// Load a named library.
static void load(const char *name)
	{
	if (eq(name,"argv")) return define_argv();
	if (eq(name,"assoc")) return use_lib("assoc.fxl");
	if (eq(name,"basic")) return define_basic();
	if (eq(name,"bn")) return define_bn();
	if (eq(name,"bool")) return use_lib("bool.fxl");
	if (eq(name,"buf")) return define_buf();
	if (eq(name,"cmp")) return define_cmp();
	if (eq(name,"core")) return define_core();
	if (eq(name,"crypto")) return define_crypto();
	if (eq(name,"date")) return use_lib("date.fxl");
	if (eq(name,"extra")) return use_lib("extra.fxl");
	if (eq(name,"fexl")) return define_fexl();
	if (eq(name,"file")) return define_file();
	if (eq(name,"format")) return use_lib("format.fxl");
	if (eq(name,"hex")) return define_hex();
	if (eq(name,"hex2")) return use_lib("hex.fxl");
	if (eq(name,"html")) return use_lib("html.fxl");
	if (eq(name,"indent")) return use_lib("indent.fxl");
	if (eq(name,"istr")) return define_istr();
	if (eq(name,"limit")) return define_limit();
	if (eq(name,"list")) return use_lib("list.fxl");
	if (eq(name,"main")) return use_lib("main.fxl");
	if (eq(name,"math")) return define_math();
	if (eq(name,"math2")) return use_lib("math.fxl");
	if (eq(name,"num")) return define_num();
	if (eq(name,"output")) return define_output();
	if (eq(name,"parse")) return define_parse();
	if (eq(name,"rand")) return define_rand();
	if (eq(name,"read")) return use_lib("read.fxl");
	if (eq(name,"read_csv")) return use_lib("read_csv.fxl");
	if (eq(name,"read_ssv")) return use_lib("read_ssv.fxl");
	if (eq(name,"record")) return define_record();
	if (eq(name,"run")) return define_run();
	if (eq(name,"run2")) return use_lib("run.fxl");
	if (eq(name,"show_value")) return use_lib("show_value.fxl");
	if (eq(name,"signal")) return define_signal();
	if (eq(name,"str")) return define_str();
	if (eq(name,"stream")) return define_stream();
	if (eq(name,"sym")) return define_sym();
	if (eq(name,"test")) return define_test();
	if (eq(name,"time")) return define_time();
	if (eq(name,"time2")) return use_lib("time.fxl");
	if (eq(name,"tuple")) return define_tuple();
	if (eq(name,"var")) return define_var();
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

	if (ch != '0')
		{
		define("use_lib",Q(type_use_lib)); // LATER 20260914 deprecated
		load("main");
		}

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
