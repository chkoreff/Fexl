#include <value.h>
#include <basic.h>
#include <fexl.h>
#include <standard.h>

/* Command line arguments */
int main_argc;
char **main_argv;

int main(int argc, char *argv[])
	{
	const char *name = argc > 1 ? argv[1] : 0;
	main_argc = argc;
	main_argv = argv;
	beg_basic();
	drop(eval_file(name));
	end_basic();
	end_value();
	return 0;
	}
