#include <stddef.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <buf.h>
#include <stream.h>
#include <type_buf.h>
#include <type_num.h>
#include <type_str.h>
#include <type_stream.h>

value type_at_eof(value fun, value f)
	{
	return boolean(cur_ch == -1);
	(void)fun;
	(void)f;
	}

value type_at_white(value fun, value f)
	{
	return boolean(at_white());
	(void)fun;
	(void)f;
	}

value type_skip_white(value fun, value f)
	{
	skip_white();
	return hold(QI);
	(void)fun;
	(void)f;
	}

value type_at_eol(value fun, value f)
	{
	return boolean(cur_ch == '\n' || cur_ch == '\r');
	(void)fun;
	(void)f;
	}

value type_at_ch(value fun, value f)
	{
	value x = f->R;
	if (x->T == type_str)
		f = boolean(cur_ch == *str_data(x));
	else
		f = hold(Qvoid);
	return f;
	(void)fun;
	}

// Return the current character.
value type_look(value fun, value f)
	{
	if (cur_ch < 0)
		return hold(Qvoid);
	else
		{
		char c = cur_ch;
		return Qstr(str_new_data(&c,1));
		}
	(void)fun;
	(void)f;
	}

// Skip to the next character.
value type_skip(value fun, value f)
	{
	skip();
	return hold(QI);
	(void)fun;
	(void)f;
	}

// Return the current line number.
value type_line(value fun, value f)
	{
	return Qnum(cur_line);
	(void)fun;
	(void)f;
	}

value type_buf_keep(value fun, value f)
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
	(void)fun;
	}

value type_collect_to_ch(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value x = arg(fun->R);
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

value type_collect_tilde_string(value fun, value f)
	{
	value x = arg(f->R);
	if (x->T == type_buf)
		f = Qnum(collect_tilde_string(get_buf(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	(void)fun;
	}

// (read_stream stream read)
value type_read_stream(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	return read_stream(arg(fun->R),hold(f->R));
	}
