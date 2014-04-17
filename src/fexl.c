#include <stdio.h>
#include <value.h>
#include <basic.h>
#include <define.h>
#include <die.h>
#include <fexl.h>
#include <parse.h>
#include <str.h>
#include <type_sym.h>

int argc;
char **argv;
char **envp;

FILE *source_fh;
const char *source_label;
int source_line;

/*TODO make a parse_file routine */
static int get_ch(void)
	{
	return fgetc(source_fh);
	}

int main(int _argc, char *_argv[], char *_envp[])
	{
	argc = _argc;
	argv = _argv;
	envp = _envp;

	beg_basic();

	source_label = argc > 1 ? argv[1] : "";
	source_fh = source_label[0] ? fopen(source_label,"r") : stdin;
	if (source_fh == 0)
		die("Could not open file %s", source_label);

	source_line = 1;
	value f = parse(get_ch,&source_line,source_label);

	value g = resolve(f,define_sym);
	if (g->T == type_sym)
		die(0); /* Die if we saw any undefined symbols. */

	value h = eval(g);
	drop(h);

	end_basic();
	end_value();
	return 0;
	}
