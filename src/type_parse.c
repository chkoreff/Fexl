#include <stdio.h>

#include <buf.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <parse.h>
#include <report.h>
#include <stream.h>
#include <type_file.h>
#include <type_istr.h>
#include <type_parse.h>
#include <type_str.h>

static value read_input(input get, void *source, value read)
	{
	struct state s = beg_stream(get,source);
	value exp = eval(read); // Run the read function
	end_stream(s);
	return exp;
	}

// Read the stream, which may be type_file, type_istr, or type_str.
static value read_stream(value stream, value read)
	{
	if (stream->T == type_file)
		return read_input((input)fgetc,stream->v_ptr,read);
	else if (stream->T == type_istr)
		return read_input((input)sgetc,stream->v_ptr,read);
	else if (stream->T == type_str)
		{
		struct istr istr;
		istr.str = stream;
		istr.pos = 0;
		return read_input((input)sgetc,&istr,read);
		}
	else
		{
		drop(read);
		return hold(Qvoid);
		}
	}

static value type_parse_top(value f)
	{
	return parse_top();
	(void)f;
	}

// Parse the Fexl stream using the given name for syntax error messages.
static value parse(value stream, value name)
	{
	value exp;
	value save_cur_label = cur_label;
	cur_label = name;
	exp = read_stream(stream,Q0(type_parse_top));
	cur_label = save_cur_label;
	drop(stream);
	return exp;
	}

static FILE *open_file(const char *name_s)
	{
	FILE *fh = fopen(name_s,"r");
	if (!fh)
		could_not_open(name_s);
	return fh;
	}

// (parse_file name) Parse the named file, or stdin if the name is empty.
// Note that if name designates a directory the fopen will succeed, but it will
// behave like an empty file.
value type_parse_file(value f)
	{
	value name = arg(f->R);
	if (name->T == type_str)
		{
		const char *name_s = str_data(name);
		value stream = Qfile(name_s[0] ? open_file(name_s) : stdin);
		f = parse(stream,name);
		}
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}

// (parse stream name) Parse the Fexl stream, using the given name for any
// syntax error messages, and return the resulting form.
value type_parse(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value name = arg(f->R);
	if (name->T == type_str)
		{
		value stream = arg(f->L->R);
		f = parse(stream,name);
		}
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}
	}

// (read_stream stream read) Read the stream with the given read function.
value type_read_stream(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value stream = arg(f->L->R);
	value read = hold(f->R);
	f = read_stream(stream,read);
	drop(stream);
	return f;
	}
	}
