#include <stdio.h>
#include <str.h>
#include <value.h>

#include <buf.h>

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

int cur_ch = -1; // current character
unsigned long cur_line = 1; // current line number
static input cur_get = empty; // current input routine
static void *cur_source; // current input source

void skip(void)
	{
	cur_ch = cur_get(cur_source);
	if (cur_ch == '\n')
		cur_line++;
	}

int at_white(void)
	{
	return cur_ch <= ' ' && cur_ch != -1;
	}

void skip_white(void)
	{
	while (at_white())
		skip();
	}

// Add the current character to the buffer.
void buf_keep(buffer buf)
	{
	buf_add(buf,cur_ch);
	skip();
	}

// Skip to end of line.
static void skip_line(void)
	{
	while (cur_ch != '\n' && cur_ch != -1)
		skip();
	}

// Skip white space and comments.
void skip_filler(void)
	{
	while (1)
		{
		if (at_white())
			skip();
		else if (cur_ch == '#')
			skip_line();
		else
			return;
		}
	}

// Collect chars to the first white space.  Return true if found.
int collect_to_white(buffer buf)
	{
	while (1)
		{
		if (cur_ch > ' ')
			buf_keep(buf);
		else if (cur_ch == -1)
			return 0;
		else
			{
			skip();
			return 1;
			}
		}
	}

// Collect chars to t_ch.  Return true if found.
int collect_to_ch(buffer buf, const char t_ch)
	{
	while (1)
		{
		if (cur_ch == t_ch)
			{
			skip();
			return 1;
			}
		else if (cur_ch == -1)
			return 0;
		else
			buf_keep(buf);
		}
	}

// Collect chars to a terminator string.  Return true if found.
int collect_string(buffer buf, const char *end, unsigned long len)
	{
	if (len == 1) // Optimize this case.
		return collect_to_ch(buf,end[0]);
	{
	unsigned long pos = 0;

	while (pos < len)
		{
		if (cur_ch == end[pos])
			{
			pos++;
			skip();
			}
		else if (pos > 0)
			{
			// Buffer the ones matched so far and start over.
			buf_addn(buf,end,pos);
			pos = 0;
			}
		else if (cur_ch == -1)
			return 0;
		else
			buf_keep(buf);
		}
	return 1;
	}
	}

// Collect chars to a white space to get a terminator string.  Then collect
// chars to the next occurrence of that terminator.  Return:
//
//   -1 : if no terminator was found
//    0 : if no occurrence of the terminator was found
//    1 : if the terminator was found
int collect_tilde_string(buffer buf)
	{
	if (!collect_to_white(buf)) return -1;
	{
	string end = buf_clear(buf);
	int ok = collect_string(buf,end->data,end->len);
	str_free(end);
	return ok;
	}
	}

// Read input, saving and restoring the context to enable nested calls.
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

	exp = eval(read); // Run the parse function

	cur_ch = save_cur_ch;
	cur_line = save_cur_line;
	cur_get = save_cur_get;
	cur_source = save_cur_source;

	return exp;
	}

// Read the stream, which may be type_file, type_istr, or type_str.
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
