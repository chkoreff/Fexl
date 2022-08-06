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

static int empty(void *source)
	{
	(void)source;
	return -1;
	}

static int cur_ch = -1; /* current character */
static unsigned long cur_line = 1; /* current line number */
static input cur_get = empty; /* current input routine */
static void *cur_source; /* current input source */

static void skip(void)
	{
	cur_ch = cur_get(cur_source);
	if (cur_ch == '\n')
		cur_line++;
	}

/* Return the current character. */
value type_look(value f)
	{
	if (cur_ch < 0)
		f = hold(Qvoid);
	else
		{
		char c = cur_ch;
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
	return Qnum(cur_line);
	}

/* Read a stream, saving and restoring the context to enable nested calls.*/
static value read_stream(input get, void *source, value read)
	{
	value exp;

	int save_cur_ch = cur_ch;
	unsigned long save_cur_line = cur_line;
	input save_cur_get = cur_get;
	void *save_cur_source = cur_source;

	cur_get = get;
	cur_source = source;
	skip();

	exp = eval(read); /* Run the parse function */

	cur_ch = save_cur_ch;
	cur_line = save_cur_line;
	cur_get = save_cur_get;
	cur_source = save_cur_source;

	return exp;
	}

/* (read_stream source read) */
value type_read_stream(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);

	if (x->T == type_istr)
		f = read_stream((input)sgetc,x->R,hold(f->R));
	else if (x->T == type_file)
		f = read_stream((input)fgetc,x->R,hold(f->R));
	else
		f = hold(Qvoid);

	drop(x);
	return f;
	}
	}
