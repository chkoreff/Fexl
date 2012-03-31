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
void string_put(value x)
	{
	fwrite(string_data(x), 1, string_len(x), stdout);
	}

/* string_put str next */
value fexl_string_put(value f)
	{
	if (!f->L->L) return f;
	value x = f->L->R;
	if (!arg(type_string,x)) return f;

	string_put(x);
	return f->R;
	}

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

/* TODO A "print" routine which detects type of arg */
