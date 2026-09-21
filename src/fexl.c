#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <load.h>
#include <report.h>
#include <string.h> // strlen
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
	define("evaluate",Q(type_evaluate));
	define_lib("load_argv",load_argv);
	define_lib("load_basic",load_basic);
	define_lib("load_bn",load_bn);
	define_lib("load_buf",load_buf);
	define_lib("load_cmp",load_cmp);
	define_lib("load_crypto",load_crypto);
	define_lib("load_extend",load_extend);
	define_lib("load_fexl",load_fexl);
	define_lib("load_file",load_file);
	define_lib("load_hex",load_hex);
	define_lib("load_istr",load_istr);
	define_lib("load_limit",load_limit);
	define_lib("load_main",load_main);
	define_lib("load_math",load_math);
	define_lib("load_num",load_num);
	define_lib("load_output",load_output);
	define_lib("load_parse",load_parse);
	define_lib("load_rand",load_rand);
	define_lib("load_record",load_record);
	define_lib("load_run",load_run);
	define_lib("load_signal",load_signal);
	define_lib("load_str",load_str);
	define_lib("load_stream",load_stream);
	define_lib("load_sym",load_sym);
	define_lib("load_test",load_test);
	define_lib("load_time",load_time);
	define_lib("load_tuple",load_tuple);
	define_lib("load_var",load_var);
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

	load_core();
	if (ch != '0') load_main();

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
