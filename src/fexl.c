#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
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
	define("load_argv",Q0(type_load_argv));
	define("load_basic",Q0(type_load_basic));
	define("load_bn",Q0(type_load_bn));
	define("load_buf",Q0(type_load_buf));
	define("load_cmp",Q0(type_load_cmp));
	define("load_crypto",Q0(type_load_crypto));
	define("load_extend",Q0(type_load_extend));
	define("load_fexl",Q0(type_load_fexl));
	define("load_file",Q0(type_load_file));
	define("load_hex",Q0(type_load_hex));
	define("load_istr",Q0(type_load_istr));
	define("load_limit",Q0(type_load_limit));
	define("load_main",Q0(type_load_main));
	define("load_math",Q0(type_load_math));
	define("load_num",Q0(type_load_num));
	define("load_output",Q0(type_load_output));
	define("load_parse",Q0(type_load_parse));
	define("load_rand",Q0(type_load_rand));
	define("load_record",Q0(type_load_record));
	define("load_run",Q0(type_load_run));
	define("load_signal",Q0(type_load_signal));
	define("load_str",Q0(type_load_str));
	define("load_stream",Q0(type_load_stream));
	define("load_sym",Q0(type_load_sym));
	define("load_test",Q0(type_load_test));
	define("load_time",Q0(type_load_time));
	define("load_tuple",Q0(type_load_tuple));
	define("load_var",Q0(type_load_var));
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
