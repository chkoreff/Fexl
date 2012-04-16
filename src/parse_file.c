#include <stdio.h>
#include "value.h"
#include "eval.h"
#include "parse.h"
#include "parse_file.h"
#include "run.h"
#include "string.h"

/* Parse directly from a FILE. */
static FILE *source_file = 0;  /* source of characters */

static int file_read_ch(void)
	{
	return fgetc(source_file);
	}

value parse_file(char *name)
	{
	source_file = name ? fopen(name,"r") : stdin;
	if (!source_file)
		error = "Can't open script";

	read_ch = file_read_ch;
	value result = parse_source();

	if (name && source_file)
		fclose(source_file);
	source_file = 0;

	return result;
	}

/* parse_file name next = (next result) */
value fexl_parse_file(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	if (!arg(type_string,x)) return f;

	return A(f->R, parse_file(string_data(x)));
	}
