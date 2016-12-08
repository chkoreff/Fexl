#include <value.h>
#include <fexl.h>
#include <type_standard.h>

int main_argc;
char **main_argv;

/* Evaluate the named file in the standard context.  Use stdin if the name is
missing or empty.  If the name designates a directory it behaves like an empty
file. */
int main(int argc, char *argv[])
	{
	const char *name = argc > 1 ? argv[1] : "";
	main_argc = argc;
	main_argv = argv;
	drop(eval_file(name));
	end_value();
	return 0;
	}
