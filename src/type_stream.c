#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <input.h>
#include <type_file.h>
#include <type_istr.h>
#include <type_num.h>
#include <type_str.h>

/* LATER 20220715 Possibly unify this with parse.c */

static int ch; /* current character */
static unsigned long line; /* current line number */
static input get; /* current input routine */
static void *source; /* current input source */

static void skip(void)
	{
	ch = get(source);
	if (ch == '\n')
		line++;
	}

/* Return the current character. */
value type_look(value f)
	{
	if (ch < 0)
		f = hold(Qvoid);
	else
		{
		char c = ch;
		f = Qstr(str_new_data(&c,1));
		}
	return f;
	}

/* Skip to the next character. */
value type_skip(value f)
	{
	(void)f;
	skip();
	return hold(QI);
	}

/* Return the current line number. */
value type_line(value f)
	{
	(void)f;
	return Qnum(line);
	}

/* Run the parse function f->R in the stream defined by _get and _source.  This
saves and restores the current stream to enable nested calls. */
static value read_stream(value f, input _get, void *_source)
	{
	int save_ch = ch;
	unsigned long save_line = line;
	input save_get = get;
	input save_source = source;

	get = _get;
	source = _source;
	line = 1;
	skip();

	f = arg(f->R); /* Run the parse function */

	ch = save_ch;
	line = save_line;
	get = save_get;
	source = save_source;

	return f;
	}

/* (read_stream source read) */
value type_read_stream(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);

	if (x->T == type_istr)
		f = read_stream(f,(input)sgetc,x->R);
	else if (x->T == type_file)
		f = read_stream(f,(input)fgetc,x->R);
	else
		f = hold(Qvoid);

	drop(x);
	return f;
	}
	}
