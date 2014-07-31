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

value new_parse_file(const char *name) /*TODO*/
	{
	source = name[0] ? fopen(name,"r") : stdin;
	if (source)
		return parse_source(get);
	else
		{
		error_code = "Could not open the input file";
		return 0;
		}
	}

/*
TODO: Perhaps close source after the parse, unless it's stdin.  However, even
if it's not stdin, you might want to continue reading source, i.e. all the data
after a terminating '\\'.  Then the question is when *does* the source get
closed?
*/
static value parse_file(string name)
	{
	FILE *save_source = source;
	value f;

	source = name->len ? fopen(name->data,"r") : stdin;
	/*TODO how would the caller see missing file? */
	f = embed_parse(source ? get : 0);

	source = save_source;
	return f;
	}

value type_parse_file(value f)
	{
	if (!f->L) return 0;
	{
	string x = atom(type_str,arg(&f->R));
	if (!x) return bad;
	return parse_file(x);
	}
	}
