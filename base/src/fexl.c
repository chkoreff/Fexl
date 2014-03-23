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

	const char *name = argc > 1 ? argv[1] : "";
	FILE *fh = name[0] ? fopen(name,"r") : stdin;
	if (fh == 0) die("Could not open file %s", name);

	long line = 1;
	value f = parse(fh,name,&line);
	f = resolve_standard(f,fh,name,line);
	f = eval(f);
	drop(f);

	end_basic();
	end_value();
	return 0;
	}
