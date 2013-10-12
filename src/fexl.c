#include <stdio.h>
#include "die.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "fexl.h"
#include "form.h"
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

	char *name = argc > 1 ? argv[1] : "";
	FILE *fh = name[0] ? fopen(name,"r") : stdin;
	if (fh == 0)
		die("Could not open file %s", name);

	value f = resolve(parse(fh,name,1));
	int exit_code = f->T == type_form ? 2 : 0;

	value g = eval(f);

	drop(f);
	drop(g);

	end_basic();
	end_value();
	return exit_code;
	}
