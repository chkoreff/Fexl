#include <stdio.h>
#include "die.h"

#include "value.h"
#include "basic.h"
#include "fexl.h"
#include "parse.h"
#include "resolve.h"

/* Save command line arguments so a Fexl program can use them. */
int argc;
char **argv;

int main(int _argc, char *_argv[])
	{
	argc = _argc;
	argv = _argv;
	beg_basic();

	source_name = argc > 1 ? argv[1] : "";
	source_fh = source_name[0] ? fopen(source_name,"r") : stdin;
	if (source_fh == 0) die("Could not open file %s", source_name);
	source_line = 1;

	drop(eval(resolve_standard(parse())));

	end_basic();
	end_value();
	return 0;
	}
