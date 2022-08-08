#include <stdio.h>
#include <str.h>
#include <value.h>

#include <buffer.h>

#include <basic.h>
#include <input.h>
#include <stream.h>
#include <type_file.h>
#include <type_istr.h>
#include <type_str.h>

static int empty(void *source)
	{
	(void)source;
	return -1;
	}

int cur_ch = -1; /* current character */
unsigned long cur_line = 1; /* current line number */
static input cur_get = empty; /* current input routine */
static void *cur_source; /* current input source */

void skip(void)
	{
	cur_ch = cur_get(cur_source);
	if (cur_ch == '\n')
		cur_line++;
	}

/* Add the current character to the buffer. */
void buf_keep(buffer buf)
	{
	buf_add(buf,(char)cur_ch);
	skip();
	}

/* Read input, saving and restoring the context to enable nested calls.*/
static value read_input(input get, void *source, value read)
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

/* Read the stream, which may be type_file, type_istr, or type_str. */
value read_stream(value stream, value read)
	{
	value exp;
	if (stream->T == type_file)
		exp = read_input((input)fgetc,stream->R,read);
	else if (stream->T == type_istr)
		exp = read_input((input)sgetc,stream->R,read);
	else if (stream->T == type_str)
		{
		struct istr istr;
		istr.str = stream;
		istr.pos = 0;
		exp = read_input((input)sgetc,&istr,read);
		}
	else
		{
		drop(read);
		exp = hold(Qvoid);
		}
	drop(stream);
	return exp;
	}
