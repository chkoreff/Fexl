#include <value.h>
#include <basic.h>
#include <die.h>
#include <output.h>
#include <parse.h>
#include <stdio.h>
#include <str.h>
#include <type_parse_file.h>
#include <type_resolve.h>
#include <type_str.h>

static FILE *source;

static int get(void)
	{
	return fgetc(source);
	}

static value parse_file(const char *name)
	{
	FILE *save_source = source;

	source = name[0] ? fopen(name,"r") : stdin;
	if (!source)
		{
		put_to_error();
		put("Could not open source file ");put(name);nl();
		die(0);
		}

	{
	value exp = parse_source(name,get);

	source = save_source;
	return exp;
	}
	}

/* (parse_file name context) = {f}, where f is the function specified in the
named file in the given context.
*/
value type_parse_file(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_str)
		{
		string name = data(x);
		value exp = parse_file(name->data);
		return single(A(A(A(Q(type_resolve), hold(x)), exp), hold(f->R)));
		}
	reduce_void(f);
	return 0;
	}
	}
