#include <string.h> /* strlen */
#include "value.h"
#include "eval.h"
#include "parse.h"
#include "string.h"

/* Parse directly from a string. */
static char *source_string = 0;  /* source of characters */
static long source_len = 0;      /* total length */
static long source_pos = 0;      /* current pos */

int string_read_ch(void)
	{
	return source_pos < source_len ? source_string[source_pos++] : -1;
	}

value parse_chars(char *string, long len)
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

/* Combinator to call parse_chars from within Fexl.
Called as (parse string next)  -- TODO resolution function, yes/no result
*/
value fexl_parse(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	if (!arg(type_string,x)) return f;

	return A(f->R,parse_chars(string_data(x), string_len(x)));
	}
