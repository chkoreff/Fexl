#include <stdio.h>
#include <str.h>
#include <value.h>

#include <buffer.h>

#include <basic.h>
#include <die.h>
#include <file.h>
#include <parse.h>
#include <stream.h>
#include <type_file.h>
#include <type_parse.h>
#include <type_str.h>

// (parse stream label) Parse the stream, using the given label for any syntax
// error messages, and return the resulting form.
value type_parse(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value label = arg(f->R);
	if (label->T == type_str)
		f = parse_fexl(arg(f->L->R),label);
	else
		f = hold(Qvoid);
	drop(label);
	return f;
	}
	}

// Parse a named file.  Note that if name designates a directory the fopen will
// succeed, but it will behave like an empty file.
static value use_file(value name)
	{
	const char *name_s = str_data(name);
	FILE *fh = name_s[0] ? fopen(name_s,"r") : stdin;
	if (!fh)
		{
		fput(stderr,"Could not open source file ");
		fput(stderr,name_s);fnl(stderr);
		die(0);
		}
	return parse_fexl(Qfile(fh),name);
	}

// (use_file name) Parse the named file.
value type_use_file(value f)
	{
	if (!f->L) return 0;
	{
	value name = arg(f->R);
	if (name->T == type_str)
		f = use_file(name);
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}
	}
