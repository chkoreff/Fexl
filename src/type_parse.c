#include <stdio.h>
#include <str.h>
#include <input.h>
#include <value.h>
#include <basic.h>
#include <die.h>
#include <file.h>
#include <parse.h>
#include <type_file.h>
#include <type_istr.h>
#include <type_parse.h>
#include <type_str.h>

static value parse_fh(FILE *fh, value label)
	{
	return parse((input)fgetc,fh,label);
	}

static value parse_istr(struct istr *istr, value label)
	{
	return parse((input)sgetc,istr,label);
	}

static value parse_string(value str, value label)
	{
	struct istr istr;
	istr.str = str;
	istr.pos = 0;
	return parse_istr(&istr,label);
	}

/* Parse a named file.  Note that if name designates a directory the fopen will
succeed, but it will behave like an empty file. */
value parse_file(value name)
	{
	const char *name_s = ((string)data(name))->data;
	FILE *fh = name_s[0] ? fopen(name_s,"r") : stdin;
	if (!fh)
		{
		fput(stderr,"Could not open source file ");
		fput(stderr,name_s);fnl(stderr);
		die(0);
		}
	return parse_fh(fh,name);
	}

/* (parse source label) Parse the source, using the given label for any syntax
error messages, and return the resulting form.  The source may be type_file,
type_str, or type_istr.
*/
value type_parse(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value source = arg(f->L->R);
	value label = arg(f->R);
	if (label->T == type_str)
		{
		if (source->T == type_file)
			f = parse_fh(data(source),hold(label));
		else if (source->T == type_str)
			f = reduce(f,parse_string(source,hold(label)));
		else if (source->T == type_istr)
			f = parse_istr(data(source),hold(label));
		else
			f = reduce_void(f);
		}
	else
		f = reduce_void(f);
	drop(source);
	drop(label);
	return f;
	}
	}
