#include <str.h>
#include <value.h>

#include <basic.h>
#include <stream.h>
#include <type_num.h>
#include <type_str.h>
#include <type_stream.h>

/* LATER 20220807 Provide (at ch) and at_eof to avoid many look calls. */

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

/* (read_stream stream read) */
value type_read_stream(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return read_stream(arg(f->L->R),hold(f->R));
	}
