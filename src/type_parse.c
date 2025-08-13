#include <stdio.h>

#include <buf.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <parse.h>
#include <parse_file.h>
#include <stream.h>
#include <type_file.h>
#include <type_istr.h>
#include <type_parse.h>
#include <type_str.h>

// (parse_file name) Parse the named file.
value type_parse_file(value f)
	{
	value name = arg(f->R);
	if (name->T == type_str)
		f = parse_file(hold(name));
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}

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
		return read_input((input)fgetc,stream->R,read);
	else if (stream->T == type_istr)
		return read_input((input)sgetc,stream->R,read);
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

// (parse stream label) Parse the stream, using the given label for any syntax
// error messages, and return the resulting form.
value type_parse(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value label = arg(f->R);
	if (label->T == type_str)
		{
		value stream = arg(f->L->R);
		value save_cur_label = cur_label;
		cur_label = label;
		f = read_stream(stream,Q0(type_parse_top));
		cur_label = save_cur_label;
		drop(stream);
		}
	else
		f = hold(Qvoid);
	drop(label);
	return f;
	}
	}

// (read_stream stream read)
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
