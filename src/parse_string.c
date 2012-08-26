#include <string.h> /* strlen */
#include "die.h"
#include "value.h"
#include "parse.h"
#include "parse_string.h"
#include "run.h"
#include "string.h"

/* Parse directly from a string. */
static char *source_string = 0;  /* source of characters */
static long source_len = 0;      /* total length */
static long source_pos = 0;      /* current pos */

static int string_read_ch(void)
	{
	return source_pos < source_len ? source_string[source_pos++] : -1;
	}

static value parse_chars(char *string, long len)
	{
	read_ch = string_read_ch;
	source_string = string;
	source_len = len;
	source_pos = 0;
	return parse_source();
	}

/* Parse a NUL-terminated string. */
value parse_string(char *string)
	{
	return parse_chars(string, strlen(string));
	}

/* (parse source) parses the source string as a Fexl function and returns
(pair ok; pair exp; symbols). */
void reduce_parse(value f)
	{
	value source = arg(type_string,f->R);
	replace(f, parse_chars(string_data(source), string_len(source)));
	}
