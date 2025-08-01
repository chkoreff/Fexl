#include <stddef.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <buf.h>
#include <crypto.h>
//#include <memory.h>
#include <show.h>
#include <stdio.h>
#include <string.h> // strcmp
#include <test.h>
#include <type_bn.h>
#include <type_buf.h>
#include <type_cmp.h>
#include <type_crypto.h>
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
#include <type_with.h>

static value Qdir_base;
static value Qdir_lib;

static value concat(value x, value y)
	{
	value f = Qstr(str_concat(x->v_ptr,y->v_ptr));
	drop(x);
	drop(y);
	return f;
	}

static value Qdirname(value x)
	{
	value f = Qstr(dirname(x->v_ptr));
	drop(x);
	return f;
	}

static void use(value name)
	{
	drop(eval(A(Q(type_evaluate),A(Q(type_parse_file),name))));
	}

static void use_lib(const char *name)
	{
	use(concat(hold(Qdir_lib),Qstr0(name)));
	}

static void beg_const(void)
	{
	QI = Q(type_I);
	QT = Q(type_T);
	QF = Q(type_F);
	QY = Q(type_Y);
	Qvoid = Q(type_void);
	Qnull = Q(type_null);
	Qonce = Q(type_once);

	Qstdin = Qfile(stdin);
	Qstdout = Qfile(stdout);
	Qstderr = Qfile(stderr);

	Qput = Q(type_put);
	Qnl = Q0(type_nl);
	Qfput = Q(type_fput);
	Qfnl = Q(type_fnl);

	Qtuple = Q(type_tuple);
	Qstd = record_empty();
	init_signal();

	// The base directory is right above the bin directory.
	Qdir_base = concat(Qdirname(Qdirname(Qstr0(main_argv[0]))),Qstr0("/"));
	Qdir_lib = concat(hold(Qdir_base),Qstr0("src/lib/"));
	}

static void end_const(void)
	{
	drop(QI);
	drop(QT);
	drop(QF);
	drop(QY);
	drop(Qvoid);
	drop(Qnull);
	drop(Qonce);

	drop(Qstdin);
	drop(Qstdout);
	drop(Qstderr);

	drop(Qput);
	drop(Qnl);
	drop(Qfput);
	drop(Qfnl);

	drop(Qtuple);
	drop(Qstd);

	close_random();
	drop(Qdir_base);
	drop(Qdir_lib);
	}

static void define_basic(void)
	{
	define("I",hold(QI));
	define("T",hold(QT));
	define("F",hold(QF));
	define("@",hold(QY));
	define("void",hold(Qvoid));
	define("null",hold(Qnull));
	define("once",hold(Qonce));
	define("cons",Q(type_cons));
	define("eval",Q(type_eval));
	define("yield",Q(type_yield));
	define("is_defined",Q(type_is_defined));
	define("is_undef",Q(type_is_undef));
	define("is_void",Q(type_is_void));
	define("is_good",Q(type_is_good));
	define("is_bool",Q(type_is_bool));
	define("is_list",Q(type_is_list));
	define("::",Q(type_chain));
	}

// big numbers
static void define_bn(void)
	{
	define("bn_eq0",Q(type_bn_eq0));
	define("bn_is_neg",Q(type_bn_is_neg));
	define("bn_neg",Q(type_bn_neg));
	define("bn_cmp",Q(type_bn_cmp));
	define("bn_lt",Q(type_bn_lt));
	define("bn_le",Q(type_bn_le));
	define("bn_eq",Q(type_bn_eq));
	define("bn_ne",Q(type_bn_ne));
	define("bn_ge",Q(type_bn_ge));
	define("bn_gt",Q(type_bn_gt));
	define("bn_from_dec",Q(type_bn_from_dec));
	define("bn_to_dec",Q(type_bn_to_dec));
	define("bn_add",Q(type_bn_add));
	define("bn_sub",Q(type_bn_sub));
	define("bn_mul",Q(type_bn_mul));
	define("bn_mod",Q(type_bn_mod));
	define("bn_div",Q(type_bn_div));
	define("is_bn",Q(type_is_bn));
	}

static void define_buf(void)
	{
	define("buf_new",Q0(type_buf_new));
	define("buf_put",Q(type_buf_put));
	define("buf_get",Q(type_buf_get));
	}

static void define_cmp(void)
	{
	define("compare",Q(type_compare));
	define("lt",Q(type_lt));
	define("le",Q(type_le));
	define("eq",Q(type_eq));
	define("ne",Q(type_ne));
	define("ge",Q(type_ge));
	define("gt",Q(type_gt));
	}

static void define_crypto(void)
	{
	define("random_bytes",Q(type_random_bytes));
	define("random_nonce",Q0(type_random_nonce));
	define("random_secret_key",Q0(type_random_secret_key));
	define("nacl_box_public",Q(type_nacl_box_public));
	define("nacl_box_prepare",Q(type_nacl_box_prepare));
	define("nacl_box_seal",Q(type_nacl_box_seal));
	define("nacl_box_open",Q(type_nacl_box_open));
	define("nacl_sign_public",Q(type_nacl_sign_public));
	define("nacl_sign_seal",Q(type_nacl_sign_seal));
	define("nacl_sign_open",Q(type_nacl_sign_open));
	define("sha256",Q(type_sha256));
	define("sha512",Q(type_sha512));
	define("pack64",Q(type_pack64));
	define("unpack64",Q(type_unpack64));
	define("hmac_sha512",Q(type_hmac_sha512));
	define("hmac_sha256",Q(type_hmac_sha256));
	}

static void define_dir(void)
	{
	define("dir_base",hold(Qdir_base));
	define("dir_lib",hold(Qdir_lib));
	}

static void define_file(void)
	{
	define("stdin",hold(Qstdin));
	define("stdout",hold(Qstdout));
	define("stderr",hold(Qstderr));
	define("fopen",Q(type_fopen));
	define("fclose",Q(type_fclose));
	define("fgetc",Q(type_fgetc));
	define("fget",Q(type_fget));
	define("clearerr",Q(type_clearerr));
	define("feof",Q(type_feof));
	define("flook",Q(type_flook));
	define("remove",Q(type_remove));
	define("is_newer",Q(type_is_newer));
	define("is_file",Q(type_is_file));
	define("is_dir",Q(type_is_dir));
	define("flock_ex",Q(type_flock_ex));
	define("flock_sh",Q(type_flock_sh));
	define("flock_un",Q(type_flock_un));
	define("readlink",Q(type_readlink));
	define("mkdir",Q(type_mkdir));
	define("rmdir",Q(type_rmdir));
	define("ftruncate",Q(type_ftruncate));
	define("fseek_set",Q(type_fseek_set));
	define("fseek_cur",Q(type_fseek_cur));
	define("fseek_end",Q(type_fseek_end));
	define("ftell",Q(type_ftell));
	define("fileno",Q(type_fileno));
	define("fread",Q(type_fread));
	define("mkfile",Q(type_mkfile));
	define("dir_names",Q(type_dir_names));
	define("mod_time",Q(type_mod_time));
	define("file_size",Q(type_file_size));
	define("symlink",Q(type_symlink));
	define("rename",Q(type_rename));
	}

static void define_hex(void)
	{
	define("unpack",Q(type_unpack));
	define("pack",Q(type_pack));
	}

static void define_istr(void)
	{
	define("readstr",Q(type_readstr));
	define("sgetc",Q(type_sgetc));
	define("sget",Q(type_sget));
	define("slook",Q(type_slook));
	}

static void define_limit(void)
	{
	define("limit_time",Q(type_limit_time));
	define("limit_stack",Q(type_limit_stack));
	define("limit_memory",Q(type_limit_memory));
	}

static void define_math(void)
	{
	define("+",Q(type_add));
	define("-",Q(type_sub));
	define("*",Q(type_mul));
	define("/",Q(type_div));
	define("^",Q(type_pow));
	define("xor",Q(type_xor));
	define("round",Q(type_round));
	define("ceil",Q(type_ceil));
	define("trunc",Q(type_trunc));
	define("abs",Q(type_abs));
	define("sqrt",Q(type_sqrt));
	define("exp",Q(type_exp));
	define("log",Q(type_log));
	define("sin",Q(type_sin));
	define("cos",Q(type_cos));
	define("pi",Qnum(num_pi));
	}

static void define_num(void)
	{
	define("num_str",Q(type_num_str));
	define("is_num",Q(type_is_num));
	}

static void define_output(void)
	{
	define("put",hold(Qput));
	define("nl",hold(Qnl));
	define("say",Q(type_say));
	define("fput",hold(Qfput));
	define("fnl",hold(Qfnl));
	define("fsay",Q(type_fsay));
	define("fflush",Q(type_fflush));
	}

static void define_parse(void)
	{
	define("parse",Q(type_parse));
	define("parse_file",Q(type_parse_file));
	}

static void define_rand(void)
	{
	define("seed_rand",Q(type_seed_rand));
	define("rand",Q0(type_rand));
	}

static void define_record(void)
	{
	define("empty",Q0(type_empty));
	define("set",Q(type_set));
	define("setf",Q(type_setf));
	define("get",Q(type_get));
	define("record_copy",Q(type_record_copy));
	define("record_count",Q(type_record_count));
	define("record_item",Q(type_record_item));
	}

static void define_run(void)
	{
	define("die",Q0(type_die));
	define("argv",Q(type_argv));
	define("sleep",Q(type_sleep));
	define("usleep",Q(type_usleep));
	define("run_process",Q(type_run_process));
	define("spawn",Q(type_spawn));
	define("start_server",Q(type_start_server));
	define("kill",Q(type_kill));
	define("connect",Q(type_connect));
	define("exec",Q(type_exec));
	define("receive_keystrokes",Q(type_receive_keystrokes));
	define("fexl_benchmark",Q(type_fexl_benchmark));
	}

static void define_signal(void)
	{
	define("set_alarm",Q(type_set_alarm));
	}

static void define_str(void)
	{
	define(".",Q(type_concat));
	define("length",Q(type_length));
	define("slice",Q(type_slice));
	define("search",Q(type_search));
	define("str_num",Q(type_str_num));
	define("ord",Q(type_ord));
	define("chr",Q(type_chr));
	define("char_width",Q(type_char_width));
	define("dirname",Q(type_dirname));
	define("basename",Q(type_basename));
	define("length_common",Q(type_length_common));
	define("compare_at",Q(type_compare_at));
	define("is_str",Q(type_is_str));
	}

static void define_stream(void)
	{
	define("at_eof",Q0(type_at_eof));
	define("at_white",Q0(type_at_white));
	define("skip_white",Q0(type_skip_white));
	define("at_eol",Q0(type_at_eol));
	define("at_ch",Q(type_at_ch));
	define("look",Q0(type_look));
	define("skip",Q0(type_skip));
	define("line",Q0(type_line));
	define("buf_keep",Q(type_buf_keep));
	define("collect_to_ch",Q(type_collect_to_ch));
	define("collect_tilde_string",Q(type_collect_tilde_string));
	define("read_stream",Q(type_read_stream));
	}

static void define_sym(void)
	{
	define("std",Q0(type_std));
	define("is_closed",Q(type_is_closed));
	define("define",Q(type_define));
	define("resolve",Q(type_resolve));
	define("evaluate",Q(type_evaluate));
	define("set_std",Q(type_set_std));
	}

static void define_time(void)
	{
	define("time",Q0(type_time));
	define("localtime",Q(type_localtime));
	define("gmtime",Q(type_gmtime));
	define("timelocal",Q(type_timelocal));
	define("timegm",Q(type_timegm));
	define("microtime",Q0(type_microtime));
	define("dow",Q(type_dow));
	}

static void define_tuple(void)
	{
	define("is_tuple",Q(type_is_tuple));
	define("tuple_to_list",Q(type_tuple_to_list));
	define("list_to_tuple",Q(type_list_to_tuple));
	}

static void define_var(void)
	{
	define("var_new",Q0(type_var_new));
	define("var_get",Q(type_var_get));
	define("var_getf",Q(type_var_getf));
	define("var_put",Q(type_var_put));
	define("var_putf",Q(type_var_putf));
	define("is_var",Q(type_is_var));
	}

static void define_with(void)
	{
	define("with",Q(type_with));
	define("is_obj",Q(type_is_obj));
	define("split_obj",Q(type_split_obj));
	define("fetch",Q(type_fetch));
	}

// Define all the functions written in C.
static void use_core(void)
	{
	define_basic();
	define_bn();
	define_buf();
	define_cmp();
	define_crypto();
	define_dir();
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
	define_var();
	define_with();
	define("show",Q(type_show));
	define("test_argv",Q(type_test_argv));
	}

/*
Evaluate the user's script.  Read the script from the file named by argv[1] if
present, or from stdin otherwise.  If the name designates a directory it
behaves like an empty file.
*/
static void eval_script(void)
	{
	const char *name_s = main_argc > 1 ? main_argv[1] : "";
	value name = Qstr0(name_s);
	define("dir_local",concat(Qdirname(hold(name)),Qstr0("/")));
	use_core();
	use_lib("main.fxl");
	use(name);
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
