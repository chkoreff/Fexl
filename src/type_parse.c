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

// Holds the current state of a stream.
struct state
	{
	int cur_ch;
	unsigned long cur_line;
	input cur_get;
	void *cur_source;
	};

static struct state get_state(void)
	{
	return (struct state){cur_ch, cur_line, cur_get, cur_source};
	}

static void put_state(struct state s)
	{
	cur_ch = s.cur_ch;
	cur_line = s.cur_line;
	cur_get = s.cur_get;
	cur_source = s.cur_source;
	}

static value type_parse_fexl(value f)
	{
	return parse_top();
	(void)f;
	}

// (parse_file name) Parse the named file.
value type_parse_file(value f)
	{
	value name = arg(f->R);
	if (name->T == type_str)
		{
		struct state s = get_state();
		f = parse_file(hold(name));
		put_state(s);
		}
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}

static value read_input(input get, void *source, value read)
	{
	cur_get = get;
	cur_source = source;
	skip();
	return eval(read); // Run the read function
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

// (parse stream label) Parse the stream, using the given label for any syntax
// error messages, and return the resulting form.
value type_parse(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value label = arg(f->R);
	if (label->T == type_str)
		{
		struct state s = get_state();
		value stream = arg(f->L->R);
		cur_label = label;
		f = read_stream(stream,Q0(type_parse_fexl));
		drop(stream);
		put_state(s);
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
	struct state s = get_state();
	value stream = arg(f->L->R);
	value read = hold(f->R);
	f = read_stream(stream,read);
	drop(stream);
	put_state(s);
	return f;
	}
	}
