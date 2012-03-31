#include "value.h"
#include "eval.h"
#include "parse_file.h"
#include "resolve.h"
#include "run.h"

/*
This is the main routine of the Fexl interpreter.  The exit status is 0 for
success, or 1 for failure.

It parses the file named on the command line, or stdin if no name is given, and
evaluates that function.
*/
int main(int argc, char *argv[], char *envp[])
	{
	beg_run(argc, argv, envp);
	value f = parse_file(argc > 1 ? argv[1] : 0, Q(fexl_resolve));
	hold(f);
	eval(f);
	drop(f);
	end_value();
	return main_exit;
	}
