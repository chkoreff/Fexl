#include <stdio.h>
#include "die.h"
#include "value.h"
#include "parse.h"
#include "parse_file.h"

/* Parse directly from a FILE. */
static FILE *source_file = 0;  /* source of characters */

int file_read_ch(void)
	{
	return fgetc(source_file);
	}

value parse_file(char *name, value resolve)
	{
	source_file = name ? fopen(name,"r") : stdin;
	if (!source_file) die("Can't open script");

	read_ch = file_read_ch;
	value f = parse_source(resolve);

	if (name) fclose(source_file);
	source_file = 0;

	return f;
	}
