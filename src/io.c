#include <stdio.h>
#include "value.h"
#include "eval.h"
#include "io.h"
#include "long.h"
#include "string.h"

void nl(void)
	{
	putchar('\n');
	}

void print(const char *s)
	{
	fputs(s, stdout);
	}

value fexl_nl(value f)
	{
	nl();
	return f->R;
	}

/* Note that we use fwrite here because we may be writing binary data.  */
void string_write(value x, FILE *stream)
	{
	fwrite(string_data(x), 1, string_len(x), stream);
	}

void string_put(value x)
	{
	string_write(x, stdout);
	}

static value op_string_write(value f, FILE *stream)
	{
	if (!f->L->L) return f;
	value x = f->L->R;
	if (!arg(type_string,x)) return f;

	string_write(x, stream);
	return f->R;
	}

/* string_put str next */
value fexl_string_put(value f)
	{
	return op_string_write(f, stdout);
	}

/* string_stderr str next */
value fexl_string_stderr(value f)
	{
	return op_string_write(f, stderr);
	}

/* (char_get next) = (next ch), where ch is the next character from stdin. */
value fexl_char_get(value f)
	{
	return A(f->R,Qlong(getchar()));
	}

/* char_put ch next */
value fexl_char_put(value f)
	{
	if (!f->L->L) return f;
	value x = f->L->R;
	if (!arg(type_long,x)) return f;

	putchar(get_long(x));
	return f->R;
	}
