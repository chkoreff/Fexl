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

void get_from_file(const char *name)
	{
	source = name[0] ? fopen(name,"r") : stdin;
	getd = source ? get : 0;
	}

/*TODO replace this once we have get_from_file, get_from_string, and
get_from_input */
/* parse_file name ok err */
value type_parse_file(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	{
	value x = eval(hold(f->L->L->R));
	value g = 0;
	if (is_atom(type_str,x))
		{
		FILE *save_source = source;
		const input save_getd = getd;

		string name = (string)x->R;
		get_from_file(name->data);
		g = embed_parse(hold(f->L->R),hold(f->R));

		source = save_source;
		getd = save_getd;
		}
	drop(x);
	return g;
	}
	}
