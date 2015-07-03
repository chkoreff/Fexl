#include <value.h>
#include <basic.h>
#include <parse.h>
#include <str.h>
#include <type_parse_string.h>
#include <type_resolve.h>
#include <type_str.h>

static string source;
static unsigned long pos;

static int get(void)
	{
	return pos < source->len ? source->data[pos++] : -1;
	}

static value parse_string(string text)
	{
	string save_source = source;
	unsigned long save_pos = pos;

	source = text;
	pos = 0;

	{
	value exp = parse_source("",get);

	source = save_source;
	pos = save_pos;
	return exp;
	}
	}

/* (parse_string str context) = {f}, where f is the function specified by the
string in the given context. */
value type_parse_string(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = eval(hold(f->L->R));
	if (x->T == type_str)
		{
		value exp = parse_string(data(x));
		f = single(
			A(A(A(Q(type_resolve), Qstr(str_new_data0(""))), exp), hold(f->R))
			);
		}
	else
		replace_void(f);

	drop(x);
	return f;
	}
	}
