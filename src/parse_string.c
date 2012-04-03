#include <string.h> /* strlen */
#include "die.h"
#include "value.h"
#include "eval.h"
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

static value parse_chars(char *string, long len, value resolve)
	{
	read_ch = string_read_ch;
	source_string = string;
	source_len = len;
	source_pos = 0;
	return parse_source(resolve);
	}

static value Parse_chars(char *string, long len)
	{
	read_ch = string_read_ch;
	source_string = string;
	source_len = len;
	source_pos = 0;
	return Parse_source();
	}

/* Parse a NUL-terminated string. */
value parse_string(char *string, value resolve)
	{
	return parse_chars(string, strlen(string), resolve);
	}

value Parse_string(char *string)
	{
	return Parse_chars(string, strlen(string));
	}

/* (parse source) parses the source string as a Fexl function and returns
(pair ok; pair exp; symbols). */
value fexl_parse(value f)
	{
	value source = f->R;
	if (!arg(type_string,source)) return f;
	return Parse_chars(string_data(source), string_len(source));
	}
