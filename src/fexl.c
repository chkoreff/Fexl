#include <str.h>
#include <value.h>

#include <basic.h>
#include <define.h>
#include <stdio.h>
#include <string.h> // strcmp
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
	define_test();
	define_time();
	define_tuple();
	define_var();
	}

/*
Evaluate the user's script.  Read the script from the file named by argv[1] if
present, or from stdin otherwise.  If the name designates a directory it
behaves like an empty file.
*/
static void eval_script(void)
	{
	define_core();
	use_lib("main.fxl");
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
