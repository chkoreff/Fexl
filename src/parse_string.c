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

static value embed_parse_string(string x)
	{
	const string save_source = source;
	const unsigned long save_pos = pos;
	value f = embed_parse(open_string(x));
	source = save_source;
	pos = save_pos;
	return f;
	}

value type_parse_string(value f)
	{
	if (!f->L) return 0;
	{
	string x = atom(type_str,arg(&f->R));
	if (!x) return bad;
	return embed_parse_string(x);
	}
	}
