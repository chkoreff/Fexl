#include <stdio.h>
#include "value.h"
#include "die.h"
#include "eval.h"
#include "parse_file.h"
#include "run.h"

/*
This is the main routine of the Fexl interpreter.  The exit status is 0 for
success, or 1 for failure.
*/
int main(int argc, char *argv[], char *envp[])
	{
	beg_run(argc, argv, envp);

	FILE *source = argc > 1 ? fopen(argv[1],"r") : stdin;
	if (!source) die("Can't open script");

	struct value *f = parse_file(source);
	if (source != stdin) fclose(source);

	hold(f);
	eval(f);
	drop(f);

	end_value();
	return 0;
	}
