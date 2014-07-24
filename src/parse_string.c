#include <value.h>
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

value parse_string(string x)
	{
	const string save_source = source;
	const unsigned long save_pos = pos;
	value f;

	source = x;
	pos = 0;

	f = parse(get);

	source = save_source;
	pos = save_pos;
	return f;
	}

value type_parse_string(value f)
	{
	if (!f->L) return 0;
	{
	string x = atom(type_str,arg(&f->R));
	if (!x) return 0;
	return parse_string(x);
	}
	}
