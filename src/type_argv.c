#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_argv.h>
#include <type_num.h>
#include <type_parse.h>
#include <type_record.h>
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

static value eval_file(value name)
	{
	return eval(A(Q(type_evaluate),A(Q(type_parse_file),name)));
	}

static void use(value name)
	{
	drop(eval_file(name));
	}

void use_file(const char *name)
	{
	use(Qstr0(name));
	}

static value eval_lib(const char *name)
	{
	return eval_file(concat(hold(Qdir_lib),Qstr0(name)));
	}

static void import_record(value cx)
	{
	struct record *rec = cx->v_ptr;
	unsigned long i;
	for (i = 0; i < rec->count; i++)
		{
		struct item *item = rec->vec + i;
		record_set(Qstd,item->key,hold(item->val));
		}
	}

void load_extend(void)
	{
	value list;
	value cx;
	value save = Qstd;
	Qstd = record_copy(Qstd);

	list = eval_lib("extend.fxl");
	cx = record_empty();
	record_fill(cx,list);

	drop(Qstd);
	Qstd = save;

	import_record(cx);
	drop(cx);
	}

void load_main(void)
	{
	drop(eval_lib("main.fxl"));
	}

void load_argv(void)
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
