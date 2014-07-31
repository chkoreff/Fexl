#include <value.h>
#include <basic.h>
#include <input.h>
#include <parse.h>
#include <parse_file.h>
#include <stdio.h>
#include <str.h>
#include <type_str.h>

static FILE *source;

static int get(void)
	{
	return fgetc(source);
	}

static input open_file(const char *name)
	{
	source = name[0] ? fopen(name,"r") : stdin;
	return source ? get : 0;
	}

value parse_file(const char *name)
	{
	return parse_source(open_file(name));
	}

static value embed_parse_file(const char *name)
	{
	FILE *save_source = source;
	value f = embed_parse(open_file(name));
	source = save_source;
	return f;
	}

value type_parse_file(value f)
	{
	if (!f->L) return 0;
	{
	string x = atom(type_str,arg(&f->R));
	if (!x) return bad;
	return embed_parse_file(x->data);
	}
	}
