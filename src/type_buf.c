#include <str.h>
#include <value.h>
#include <basic.h>
#include <buffer.h>
#include <type_buf.h>
#include <type_str.h>

value type_buf(value f)
	{
	return type_void(f);
	}

/* buf_new returns a new empty character buffer. */
value type_buf_new(value f)
	{
	(void)f;
	return D(type_buf,buf_new(),(type)buf_free);
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
		buffer *buf = data(x);
		string str = data(y);
		buf_put(buf,str);
		f = QI();
		}
	else
		f = reduce_void(f);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (buf_get buf) Clear the buffer and return str, where str is the current
content of the buffer. */
value type_buf_get(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_buf)
		{
		buffer *buf = data(x);
		string str = buf_clear(buf);
		f = Qstr(str);
		}
	else
		f = reduce_void(f);
	drop(x);
	return f;
	}
	}
