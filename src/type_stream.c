#include <str.h>
#include <value.h>

#include <buffer.h>

#include <basic.h>
#include <stream.h>
#include <type_buf.h>
#include <type_num.h>
#include <type_str.h>
#include <type_stream.h>

value type_at_eof(value f)
	{
	(void)f;
	return boolean(cur_ch == -1);
	}

value type_at_white(value f)
	{
	(void)f;
	return boolean(at_white());
	}

value type_skip_white(value f)
	{
	(void)f;
	skip_white();
	return hold(QI);
	}

value type_at_eol(value f)
	{
	(void)f;
	return boolean(cur_ch == '\n' || cur_ch == '\r');
	}

value type_at_ch(value f)
	{
	if (!f->L) return 0;
	{
	value x = f->R;
	if (x->T == type_str)
		f = boolean(cur_ch == *str_data(x));
	else
		f = hold(Qvoid);
	return f;
	}
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

value type_buf_keep(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_buf)
		{
		buf_keep(get_buf(x));
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value type_collect_to_ch(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_buf && y->T == type_str)
		f = boolean(collect_to_ch(get_buf(x),str_data(y)[0]));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

value type_collect_tilde_string(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_buf)
		f = Qnum(collect_tilde_string(get_buf(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (read_stream stream read) */
value type_read_stream(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return read_stream(arg(f->L->R),hold(f->R));
	}
