#include <stdio.h>

#include <buf.h>
#include <str.h>
#include <value.h>

#include <parse.h>
#include <parse_file.h>
#include <report.h>
#include <stream.h>
#include <type_str.h>

static value parse_fh(FILE *fh, value name)
	{
	cur_label = name;
	cur_get = (input)fgetc;
	cur_source = fh;
	skip();

	{
	value exp = parse_top();
	drop(cur_label);
	return exp;
	}
	}

static FILE *open_file(const char *name_s)
	{
	FILE *fh = fopen(name_s,"r");
	if (!fh)
		could_not_open(name_s);
	return fh;
	}

// Parse a named file, or stdin if the name is empty.
// Note that if name designates a directory the fopen will succeed, but it will
// behave like an empty file.
value parse_file(value name)
	{
	const char *name_s = str_data(name);
	if (name_s[0])
		{
		FILE *fh = open_file(name_s);
		value exp = parse_fh(fh,name);
		fclose(fh);
		return exp;
		}
	else
		return parse_fh(stdin,name);
	}
