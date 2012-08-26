#include <stdio.h>
#include "value.h"
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

/* Parse the file with the given name.  If name is 0 or empty, use stdin. */
value parse_file(char *name)
	{
	source_file = name && name[0] ? fopen(name,"r") : stdin;
	read_ch = source_file ? file_read_ch : 0;

	value result = parse_source();

	if (source_file && source_file != stdin)
		fclose(source_file);

	source_file = 0;
	return result;
	}

/* parse_file name next = (next result)
TODO perhaps simply return the value like "parse" (a string)
*/
static void reduce2_parse_file(value f)
	{
	value x = arg(type_string,f->L->R);
	replace_apply(f, f->R, parse_file(string_data(x)));
	}

void reduce_parse_file(value f)
	{
	f->T = reduce2_parse_file;
	}
