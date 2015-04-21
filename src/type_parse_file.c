#include <value.h>
#include <basic.h>
#include <die.h>
#include <input.h>
#include <output.h>
#include <parse.h>
#include <stdio.h>
#include <str.h>
#include <type_parse_file.h>
#include <type_str.h>
#include <type_sym.h>

static FILE *source;

static int get(void)
	{
	return fgetc(source);
	}

static value parse_file(const char *name)
	{
	value exp;
	input save_getd = getd;
	FILE *save_source = source;

	source = name && name[0] ? fopen(name,"r") : stdin;
	getd = source ? get : 0;

	if (!getd)
		{
		put_to_error();
		put("Could not open source file ");put(name);nl();
		die(0);
		}

	exp = parse_source(name);

	getd = save_getd;
	source = save_source;
	return exp;
	}

value type_parse_file(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		string name = data(x);
		value exp = parse_file(name->data);
		replace_A(f, A(Q(type_resolve),hold(x)), exp);
		}
	else
		replace_void(f);

	drop(x);
	return 0;
	}
	}
