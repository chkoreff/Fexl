#include <str.h>
#include <value.h>

#include <buf.h>

#include <basic.h>
#include <crypto.h>
#include <stdio.h>
#include <string.h> // strcmp
#include <type_bn.h>
#include <type_buf.h>
#include <type_cmp.h>
#include <type_crypto.h>
#include <type_file.h>
#include <type_hex.h>
#include <type_istr.h>
#include <type_lib.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_parse.h>
#include <type_rand.h>
#include <type_record.h>
#include <type_run.h>
#include <type_signal.h>
#include <type_standard.h>
#include <type_str.h>
#include <type_stream.h>
#include <type_sym.h>
#include <type_time.h>
#include <type_tuple.h>
#include <type_var.h>
#include <type_with.h>

// Resolve standard names.
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
	if (match("ceil")) return Q(type_ceil);
	if (match("trunc")) return Q(type_trunc);
	if (match("abs")) return Q(type_abs);
	if (match("sqrt")) return Q(type_sqrt);
	if (match("exp")) return Q(type_exp);
	if (match("log")) return Q(type_log);
	if (match("sin")) return Q(type_sin);
	if (match("cos")) return Q(type_cos);
	if (match("pi")) return Qnum(num_pi);

	if (match("compare")) return Q(type_compare);
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
	if (match("cons")) return Q(type_cons);
	if (match("null")) return hold(Qnull);
	if (match("eval")) return Q(type_eval);
	if (match("yield")) return hold(Qyield);

	if (match("is_defined")) return Q(type_is_defined);
	if (match("is_undef")) return Q(type_is_undef);
	if (match("is_void")) return Q(type_is_void);
	if (match("is_good")) return Q(type_is_good);
	if (match("is_bool")) return Q(type_is_bool);
	if (match("is_list")) return Q(type_is_list);
	if (match("chain")) return Q(type_chain);

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
	if (match("length_common")) return Q(type_length_common);
	if (match("compare_at")) return Q(type_compare_at);
	if (match("is_str")) return Q(type_is_str);
	if (match("with")) return Q(type_with);
	if (match("is_obj")) return Q(type_is_obj);
	if (match("split_obj")) return Q(type_split_obj);
	if (match("fetch")) return Q(type_fetch);

	if (match("num_str")) return Q(type_num_str);
	if (match("is_num")) return Q(type_is_num);

	if (match("is_tuple")) return Q(type_is_tuple);
	if (match("tuple_to_list")) return Q(type_tuple_to_list);
	if (match("list_to_tuple")) return Q(type_list_to_tuple);

	if (match("stdin")) return hold(Qstdin);
	if (match("stdout")) return hold(Qstdout);
	if (match("stderr")) return hold(Qstderr);
	if (match("fopen")) return Q(type_fopen);
	if (match("fclose")) return Q(type_fclose);
	if (match("fgetc")) return Q(type_fgetc);
	if (match("fget")) return Q(type_fget);
	if (match("clearerr")) return Q(type_clearerr);
	if (match("feof")) return Q(type_feof);
	if (match("flook")) return Q(type_flook);
	if (match("remove")) return Q(type_remove);
	if (match("is_newer")) return Q(type_is_newer);
	if (match("is_file")) return Q(type_is_file);
	if (match("is_dir")) return Q(type_is_dir);
	if (match("flock_ex")) return Q(type_flock_ex);
	if (match("flock_sh")) return Q(type_flock_sh);
	if (match("flock_un")) return Q(type_flock_un);
	if (match("readlink")) return Q(type_readlink);
	if (match("mkdir")) return Q(type_mkdir);
	if (match("rmdir")) return Q(type_rmdir);
	if (match("ftruncate")) return Q(type_ftruncate);
	if (match("fseek_set")) return Q(type_fseek_set);
	if (match("fseek_cur")) return Q(type_fseek_cur);
	if (match("fseek_end")) return Q(type_fseek_end);
	if (match("ftell")) return Q(type_ftell);
	if (match("fileno")) return Q(type_fileno);
	if (match("fread")) return Q(type_fread);
	if (match("mkfile")) return Q(type_mkfile);
	if (match("dir_names")) return Q(type_dir_names);
	if (match("mod_time")) return Q(type_mod_time);
	if (match("file_size")) return Q(type_file_size);
	if (match("symlink")) return Q(type_symlink);
	if (match("rename")) return Q(type_rename);

	// stream
	if (match("at_eof")) return Q0(type_at_eof);
	if (match("at_white")) return Q0(type_at_white);
	if (match("skip_white")) return Q0(type_skip_white);
	if (match("at_eol")) return Q0(type_at_eol);
	if (match("at_ch")) return Q(type_at_ch);
	if (match("look")) return Q0(type_look);
	if (match("skip")) return Q0(type_skip);
	if (match("line")) return Q0(type_line);
	if (match("buf_keep")) return Q(type_buf_keep);
	if (match("collect_to_ch")) return Q(type_collect_to_ch);
	if (match("collect_tilde_string")) return Q(type_collect_tilde_string);
	if (match("read_stream")) return Q(type_read_stream);

	if (match("time")) return Q0(type_time);
	if (match("localtime")) return Q(type_localtime);
	if (match("gmtime")) return Q(type_gmtime);
	if (match("timelocal")) return Q(type_timelocal);
	if (match("timegm")) return Q(type_timegm);
	if (match("microtime")) return Q0(type_microtime);
	if (match("dow")) return Q(type_dow);

	if (match("die")) return Q0(type_die);
	if (match("argv")) return Q(type_argv);
	if (match("sleep")) return Q(type_sleep);
	if (match("usleep")) return Q(type_usleep);
	if (match("run_process")) return Q(type_run_process);
	if (match("spawn")) return Q(type_spawn);
	if (match("exec")) return Q(type_exec);
	if (match("fexl_benchmark")) return Q(type_fexl_benchmark);

	if (match("seed_rand")) return Q(type_seed_rand);
	if (match("rand")) return Q0(type_rand);

	if (match("parse")) return Q(type_parse);
	if (match("use_file")) return Q(type_use_file);

	if (match("is_closed")) return Q(type_is_closed);
	if (match("def")) return Q(type_def);
	if (match("std")) return Q(type_std);
	if (match("value")) return Q(type_value);
	if (match("resolve")) return Q(type_resolve);

	if (match("buf_new")) return Q0(type_buf_new);
	if (match("buf_put")) return Q(type_buf_put);
	if (match("buf_get")) return Q(type_buf_get);

	if (match("readstr")) return Q(type_readstr);
	if (match("sgetc")) return Q(type_sgetc);
	if (match("sget")) return Q(type_sget);
	if (match("slook")) return Q(type_slook);

	if (match("var_new")) return Q0(type_var_new);
	if (match("var_get")) return Q(type_var_get);
	if (match("var_getf")) return Q(type_var_getf);
	if (match("var_put")) return Q(type_var_put);
	if (match("var_putf")) return Q(type_var_putf);
	if (match("is_var")) return Q(type_is_var);

	if (match("limit_time")) return Q(type_limit_time);
	if (match("limit_memory")) return Q(type_limit_memory);
	if (match("limit_stack")) return Q(type_limit_stack);

	if (match("unpack")) return Q(type_unpack);
	if (match("pack")) return Q(type_pack);

	// record
	if (match("empty")) return hold(Qempty);
	if (match("set")) return Q(type_set);
	if (match("setf")) return Q(type_setf);
	if (match("get")) return Q(type_get);
	if (match("record_count")) return Q(type_record_count);
	if (match("record_item")) return Q(type_record_item);

	// crypto functions
	if (match("random_bytes")) return Q(type_random_bytes);
	if (match("random_nonce")) return Q0(type_random_nonce);
	if (match("random_secret_key")) return Q0(type_random_secret_key);
	if (match("nacl_box_public")) return Q(type_nacl_box_public);
	if (match("nacl_box_prepare")) return Q(type_nacl_box_prepare);
	if (match("nacl_box_seal")) return Q(type_nacl_box_seal);
	if (match("nacl_box_open")) return Q(type_nacl_box_open);
	if (match("nacl_sign_public")) return Q(type_nacl_sign_public);
	if (match("nacl_sign_seal")) return Q(type_nacl_sign_seal);
	if (match("nacl_sign_open")) return Q(type_nacl_sign_open);
	if (match("sha256")) return Q(type_sha256);
	if (match("sha512")) return Q(type_sha512);
	if (match("pack64")) return Q(type_pack64);
	if (match("unpack64")) return Q(type_unpack64);
	if (match("hmac_sha512")) return Q(type_hmac_sha512);
	if (match("hmac_sha256")) return Q(type_hmac_sha256);

	// big numbers
	if (match("bn_eq0")) return Q(type_bn_eq0);
	if (match("bn_is_neg")) return Q(type_bn_is_neg);
	if (match("bn_neg")) return Q(type_bn_neg);
	if (match("bn_cmp")) return Q(type_bn_cmp);
	if (match("bn_lt")) return Q(type_bn_lt);
	if (match("bn_le")) return Q(type_bn_le);
	if (match("bn_eq")) return Q(type_bn_eq);
	if (match("bn_ne")) return Q(type_bn_ne);
	if (match("bn_ge")) return Q(type_bn_ge);
	if (match("bn_gt")) return Q(type_bn_gt);
	if (match("bn_from_dec")) return Q(type_bn_from_dec);
	if (match("bn_to_dec")) return Q(type_bn_to_dec);
	if (match("bn_add")) return Q(type_bn_add);
	if (match("bn_sub")) return Q(type_bn_sub);
	if (match("bn_mul")) return Q(type_bn_mul);
	if (match("bn_mod")) return Q(type_bn_mod);
	if (match("bn_div")) return Q(type_bn_div);
	if (match("is_bn")) return Q(type_is_bn);

	if (match("set_alarm")) return Q(type_set_alarm);
	if (match("start_server")) return Q(type_start_server);
	if (match("kill")) return Q(type_kill);
	if (match("connect")) return Q(type_connect);
	if (match("receive_keystrokes")) return Q(type_receive_keystrokes);

	// dynamic libraries
	if (match("dlopen")) return Q(type_dlopen);
	if (match("dlerror")) return Q0(type_dlerror);
	if (match("dlsym")) return Q(type_dlsym);

	return 0;
	}

value type_std(value fun, value f)
	{
	return op_resolve(fun,f,standard);
	}

static void beg_const(void)
	{
	beg_basic();
	beg_file();
	beg_output();
	beg_tuple();
	beg_record();
	init_signal();
	}

static void end_const(void)
	{
	end_basic();
	end_file();
	end_output();
	end_tuple();
	end_record();
	close_random();
	}

// Evaluate the user's script.  Read the script from the file named by argv[1]
// if present, or from stdin otherwise.
//
// If this program is running as "fexl0", it runs your script directly.  If it
// is running as "fexl", it first runs the local script "src/main.fxl", which
// then runs your script within an extended library context.
//
// The purpose of "fexl0" is to give you the option of bypassing src/main.fxl
// altogether, defining any extensions beyond the built-in C standard context
// as you please.
//
// In a future release I will make fexl0 the default behavior, so it always
// reads your script directly without loading any default library.  Predefined
// libraries such as the current main.fxl will be provided in the lib directory
// so you can pull them in with a one-liner.
//
// I am making this change because the one-size-fits-all approach to a standard
// library is a losing proposition.  Eventually I might even factor out the
// currently built-in C functions into independent libraries loaded on demand.
// Then you could build an entirely separate C library, put it in your own
// directory somewhere, and load it from within Fexl.  That would even give you
// the power to generate binary code on the fly and run it immediately.

static void eval_script(void)
	{
	value f;
	unsigned long len = strlen(main_argv[0]);
	if (len >= 5 && strcmp(main_argv[0]+len-5,"fexl0") == 0)
		{
		// Running as fexl0, so run the user's script directly.
		const char *name = main_argc > 1 ? main_argv[1] : "";
		f = Qstr0(name);
		}
	else
		{
		// Get the name of the currently running executable.
		f = Qstr0(main_argv[0]);
		// Go two directories up, right above the bin directory.
		f = A(Q(type_dirname),f);
		f = A(Q(type_dirname),f);
		// Concatenate the name of the main script.
		f = A(A(Q(type_concat),f),Qstr0("/src/main.fxl"));
		}

	// Now evaluate the script.
	f = A(Q(type_use_file),f);
	f = A(Q(type_std),f);
	f = A(Q(type_value),f);
	f = eval(f);
	drop(f);
	}

void eval_standard(int argc, const char *argv[])
	{
	main_argc = argc;
	main_argv = argv;
	beg_const();
	eval_script();
	end_const();
	end_value();
	}
