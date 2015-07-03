#include <value.h>
#include <basic.h>
#include <str.h>
#include <top.h>
#include <type_parse_file.h>
#include <type_str.h>

/* Command line arguments */
int main_argc;
char **main_argv;

/* Evaluate the named file in the context.  Use stdin if the name is null or
empty. */
static value eval_file(const char *name, value context)
	{
	value label = Qstr(str_new_data0(name));
	return eval(A(A(A(Q(type_parse_file),label),context),Q(type_I)));
	}

int eval_top(int argc, char *argv[], value context)
	{
	const char *name = argc > 1 ? argv[1] : "";
	main_argc = argc;
	main_argv = argv;
	drop(eval_file(name,context));
	end_value();
	return 0;
	}
