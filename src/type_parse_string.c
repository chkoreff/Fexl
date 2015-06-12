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

/* (parse_string str) */
value type_parse_string(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		value exp = parse_string(data(x));
		replace_A(f, A(Q(type_resolve),Qstr(str_new_data0(""))), exp);
		}
	else
		replace_void(f);

	drop(x);
	return 0;
	}
	}
