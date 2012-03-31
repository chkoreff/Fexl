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

value parse_chars(char *string, long len, value resolve)
	{
	read_ch = string_read_ch;
	source_string = string;
	source_len = len;
	source_pos = 0;
	return parse_source(resolve);
	}

/* Parse a NUL-terminated string. */
value parse_string(char *string, value resolve)
	{
	return parse_chars(string, strlen(string), resolve);
	}

/* Combinator to call parse_chars from within Fexl.
Called as (parse string resolve next), where the resolve function supplies
definitions for outer symbols used in the string.
*/
value fexl_parse(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;
	if (!arg(type_string,x)) return f;

	value resolve = f->L->R;

	return A(f->R,parse_chars(string_data(x), string_len(x), resolve));
	}
