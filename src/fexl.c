#include <stddef.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <core_20240529.h>
#include <die.h>
#include <fexl.h>
#include <stdio.h>
#include <type_parse.h>
#include <type_str.h>
#include <type_sym.h>

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

// Return the named context from the environment.
static value read_context(value name)
	{
	cur_name = str_data(name);

	// core_20240529 = core C routines
	if (match("core_20240529")) return Q(type_core_20240529);

	// std_20240529 = core_20240529 + lib extensions
	if (match("std_20240529")) return read_env("std_20240529.fxl");

	// test_20240529 = std_20240529 + test routines
	if (match("test_20240529")) return read_env("test_20240529.fxl");

	fprintf(stderr, "Undefined context \"%s\"\n", cur_name);
	die(0);
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
