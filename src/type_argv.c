#include <str.h>
#include <value.h>

#include <basic.h>
#include <define.h>
#include <type_argv.h>
#include <type_num.h>
#include <type_parse.h>
#include <type_str.h>
#include <type_sym.h>

int main_argc;
const char **main_argv;

static value Qdir_base;
static value Qdir_lib;

// (argv i) Return the command line argument at position i (starting at 0), or
// void if no such position.
static value type_argv(value f)
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		int i = x->v_double;
		if (i >= 0 && i < main_argc)
			f = Qstr(str_new_data0(main_argv[i]));
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

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

void use_file(const char *name)
	{
	use(Qstr0(name));
	}

void use_lib(const char *name)
	{
	use(concat(hold(Qdir_lib),Qstr0(name)));
	}

void define_argv(void)
	{
	define("argv",Q(type_argv));
	define("dir_base",hold(Qdir_base));
	define("dir_lib",hold(Qdir_lib));
	}

void beg_argv(void)
	{
	// The base directory is two levels above the bin/fexl program.
	Qdir_base = concat(Qdirname(Qdirname(Qstr0(main_argv[0]))),Qstr0("/"));

	// The lib directory is src/lib under the base directory.
	Qdir_lib = concat(hold(Qdir_base),Qstr0("src/lib/"));
	}

void end_argv(void)
	{
	drop(Qdir_base);
	drop(Qdir_lib);
	}
