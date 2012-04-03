#include <stdio.h>
#include "value.h"
#include "parse.h"
#include "parse_file.h"
#include "run.h"

/* Parse directly from a FILE. */
static FILE *source_file = 0;  /* source of characters */

static int file_read_ch(void)
	{
	return fgetc(source_file);
	}

value parse_file(char *name, value resolve)
	{
	source_file = name ? fopen(name,"r") : stdin;
	if (!source_file)
		{
		error = "Can't open script";
		return 0;
		}

	read_ch = file_read_ch;
	value f = parse_source(resolve);

	if (name) fclose(source_file);
	source_file = 0;

	return f;
	}

value Parse_file(char *name)
	{
	source_file = name ? fopen(name,"r") : stdin;
	if (!source_file)
		{
		error = "Can't open script";
		return 0;
		}

	read_ch = file_read_ch;
	value result = Parse_source();

	if (name) fclose(source_file);
	source_file = 0;

	return result;
	}
