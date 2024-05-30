#include <dlfcn.h>
#include <stddef.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <crypto.h> // TODO close_random
#include <die.h>
#include <fexl.h>
#include <stdio.h>
#include <type_file.h>
#include <type_output.h>
#include <type_parse.h>
#include <type_record.h>
#include <type_signal.h> // TODO init_signal
#include <type_str.h>
#include <type_sym.h>
#include <type_tuple.h>

int main_argc;
const char **main_argv;

value Qdir_base;
value Qconcat;
value Qvalue;
value Quse_file;
static value Qboot; // boot environment

static value concat(value x, value y)
	{
	return A(A(hold(Qconcat),x),y);
	}

static value read_path(value path)
	{
	value f;
	f = A(hold(Quse_file),path); // Parse the file.
	f = A(A(hold(Qvalue),hold(Qvoid)),f); // Evaluate in void context.
	f = A(f,hold(Qboot)); // Pass in the boot environment.
	return eval(f);
	}

static value read_env(const char *name)
	{
	value dir_lib = concat(hold(Qdir_base),Qstr0("/src/env/"));
	value path = concat(dir_lib,Qstr0(name));
	return read_path(path);
	}

static value read_lib(const char *s_path, const char *sym_name)
	{
	value cx;
	void *lib = dlopen(s_path,RTLD_NOW|RTLD_NODELETE);
	if (lib == 0)
		die(dlerror());
	{
	type t = dlsym(lib,sym_name);
	if (t == 0)
		die(dlerror());
	cx = Q(t);
	dlclose(lib);
	return cx;
	}
	}

static value read_base_lib(const char *lib_name, const char *sym_name)
	{
	value dir_lib = eval(concat(hold(Qdir_base),Qstr0("/lib/")));
	value path = eval(concat(dir_lib,Qstr0(lib_name)));
	const char *s_path = str_data(path);
	value cx = read_lib(s_path,sym_name);
	drop(path);
	return cx;
	}

static void undefined_context(void)
	{
	fprintf(stderr, "Undefined context \"%s\"\n", cur_name);
	die(0);
	}

// Return the named context from the environment.
static value read_context(value name)
	{
	cur_name = str_data(name);

	// core_20240529 = core C routines

	if (match("core_20240529"))
		return read_base_lib("core_20240529.so","type_cx_core_20240529");

	// std_20240529 = core_20240529 + lib extensions
	if (match("std_20240529"))
		return read_env("std_20240529.fxl");

	// test_20240529 = std_20240529 + test routines
	if (match("test_20240529"))
		return read_env("test_20240529.fxl");

	undefined_context();
	return 0;
	}

static value type_boot(value f)
	{
	value name = arg(f->R);
	if (name->T == type_str)
		f = A(hold(Qvalue),read_context(name));
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}

static value load_context(value name)
	{
	cur_name = str_data(name);

	if (match("show"))
		return read_base_lib("show.so","type_cx_show");
	if (match("test"))
		return read_base_lib("test.so","type_cx_test");
	if (match("core_20240529"))
		return read_base_lib("core_20240529.so","type_cx_core_20240529");

	undefined_context();
	return 0;
	}

value type_load(value f)
	{
	value name = arg(f->R);
	if (name->T == type_str)
		f = load_context(name);
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}

static void beg_core(void)
	{
	Qdir_base =
		eval(A(Q(type_dirname),A(Q(type_dirname),Qstr0(main_argv[0]))));
	Qconcat = Q(type_concat);
	Qvalue = Q(type_value);
	Quse_file = Q(type_use_file);
	Qboot = Q(type_boot);
	}

static void end_core(void)
	{
	drop(Qdir_base);
	drop(Qconcat);
	drop(Qvalue);
	drop(Quse_file);
	drop(Qboot);
	}

static void beg_core_20240529(void)
	{
	beg_file();
	beg_output();
	beg_tuple();
	beg_record();
	init_signal();
	}

static void end_core_20240529(void)
	{
	end_file();
	end_output();
	end_tuple();
	end_record();
	close_random();
	}

static void beg_const(void)
	{
	beg_core();
	beg_basic();
	beg_core_20240529();
	}

static void end_const(void)
	{
	end_core();
	end_basic();
	end_core_20240529();
	}

/*
Evaluate the named file.  Use stdin if the name is missing or empty.  If the
name designates a directory it behaves like an empty file.
*/
int main(int argc, const char *argv[])
	{
	value path = Qstr0(argc > 1 ? argv[1] : "");
	main_argc = argc;
	main_argv = argv;
	beg_const();
	drop(read_path(path));
	end_const();
	end_value();
	return 0;
	}
