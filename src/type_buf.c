#include <str.h>
#include <value.h>
#include <basic.h>
#include <buffer.h>
#include <type_buf.h>
#include <type_str.h>

value type_buf(value f)
	{
	if (f->N == 0)
		{
		buf_free((buffer *)f->R);
		return 0;
		}
	return type_void(f);
	}

/* buf_new yields a new empty character buffer. */
value type_buf_new(value f)
	{
	(void)f;
	action = 1;
	return yield(D(type_buf,buf_new()));
	}

/* (buf_put buf str) Appends the string to the buffer. */
value type_buf_put(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_buf && y->T == type_str)
		{
		buffer *buf = (buffer *)x->R;
		string str = (string)y->R;
		buf_put(buf,str);
		action = 1;
		f = QI();
		}
	else
		reduce_void(f);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (buf_get buf) Clear the buffer and yield str, where str is the current
content of the buffer. */
value type_buf_get(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_buf)
		{
		buffer *buf = (buffer *)x->R;
		string str = buf_clear(buf);
		action = 1;
		f = yield(Qstr(str));
		}
	else
		reduce_void(f);
	drop(x);
	return f;
	}
	}
