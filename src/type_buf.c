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

/* buf_new = {buf}, where buf is a new empty character buffer. */
value type_buf_new(value f)
	{
	buffer buf = 0;
	(void)f;
	return A(Q(type_single), D(type_buf,buf,(type)buf_free));
	}

/* (buf_put buf str) Appends the string to the buffer. */
value type_buf_put(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = eval(hold(f->L->R));
	value y = eval(hold(f->R));
	if (x->T == type_buf && y->T == type_str)
		{
		buffer buf = data(x);
		string str = data(y);
		buf = buf_put(buf,str);
		x->R->R = (value)buf;
		f = Q(type_I);
		}
	else
		replace_void(f);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (buf_get buf) Clear the buffer and return {str}, where str is the current
content of the buffer. */
value type_buf_get(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_buf)
		{
		buffer buf = data(x);
		string str = buf_finish(buf);
		x->R->R = 0;
		f = A(Q(type_single),Qstr(str));
		}
	else
		replace_void(f);
	drop(x);
	return f;
	}
	}
