#include <value.h>
#include <basic.h>
#include <input.h>
#include <str.h>
#include <parse.h>
#include <parse_string.h>
#include <type_str.h>

static string source;
static unsigned long pos;

static int get(void)
	{
	return pos < source->len ? source->data[pos++] : -1;
	}

static input open_string(string x)
	{
	source = x;
	pos = 0;
	return get;
	}

/* parse_string str ok err */
value type_parse_string(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	{
	value x = eval(hold(f->L->L->R));
	value g = 0;
	if (is_atom(type_str,x))
		{
		const string save_source = source;
		const input save_getd = getd;
		const unsigned long save_pos = pos;

		getd = open_string((string)x->R);
		g = embed_parse(hold(f->L->R),hold(f->R));

		source = save_source;
		pos = save_pos;
		getd = save_getd;
		}
	drop(x);
	return g;
	}
	}
