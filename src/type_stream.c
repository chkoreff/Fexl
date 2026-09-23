#include <stddef.h> // size_t
#include <str.h>
#include <value.h>

#include <buf.h>

#include <basic.h>
#include <stream.h>
#include <type_buf.h>
#include <type_num.h>
#include <type_parse.h>
#include <type_str.h>
#include <type_stream.h>

static value type_at_eof(value f)
	{
	return boolean(cur_ch == -1);
	(void)f;
	}

static value type_at_white(value f)
	{
	return boolean(at_white());
	(void)f;
	}

static value type_skip_white(value f)
	{
	skip_white();
	return hold(QI);
	(void)f;
	}

static value type_at_eol(value f)
	{
	return boolean(cur_ch == '\n' || cur_ch == '\r');
	(void)f;
	}

static value type_at_ch(value f)
	{
	value x = f->R;
	if (x->T == type_str)
		f = boolean(cur_ch == *str_data(x));
	else
		f = hold(Qvoid);
	return f;
	}

// Return the current character.
static value type_look(value f)
	{
	if (cur_ch < 0)
		return hold(Qvoid);
	else
		{
		char c = cur_ch;
		return Qstr(str_new_data(&c,1));
		}
	(void)f;
	}

// Skip to the next character.
static value type_skip(value f)
	{
	skip();
	return hold(QI);
	(void)f;
	}

// Return the current line number.
static value type_line(value f)
	{
	return Qnum(cur_line);
	(void)f;
	}

static value type_buf_keep(value f)
	{
	value x = arg(f->R);
	if (x->T == type_buf)
		{
		buf_keep(x->v_ptr);
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value type_collect_to_ch(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_buf && y->T == type_str)
		f = boolean(collect_to_ch(x->v_ptr,str_data(y)[0]));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static value type_collect_tilde_string(value f)
	{
	value x = arg(f->R);
	if (x->T == type_buf)
		f = Qnum(collect_tilde_string(x->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

value type_load_stream(value f)
	{
	define("at_eof",Q0(type_at_eof));
	define("at_white",Q0(type_at_white));
	define("skip_white",Q0(type_skip_white));
	define("at_eol",Q0(type_at_eol));
	define("at_ch",Q(type_at_ch));
	define("look",Q0(type_look));
	define("skip",Q0(type_skip));
	define("line",Q0(type_line));
	define("buf_keep",Q(type_buf_keep));
	define("collect_to_ch",Q(type_collect_to_ch));
	define("collect_tilde_string",Q(type_collect_tilde_string));
	define("read_stream",Q(type_read_stream));
	return hold(QI);
	(void)f;
	}
