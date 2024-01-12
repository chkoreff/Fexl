#include <stddef.h>
#include <str.h>
#include <value.h>

#include <buf.h>

#include <basic.h>
#include <buf_str.h>
#include <memory.h>
#include <type_buf.h>
#include <type_str.h>

buffer get_buf(value x)
	{
	return x->v_ptr;
	}

value type_buf(value f)
	{
	return type_void(f);
	}

static void clear_buf(value f)
	{
	buffer buf = f->v_ptr;
	buf_discard(buf);
	free_memory(buf,sizeof(struct buffer));
	}

// buf_new returns a new empty character buffer.
value type_buf_new(value f)
	{
	static struct value clear = {{.N=0}, {.clear=clear_buf}};
	buffer buf = new_memory(sizeof(struct buffer));
	*buf = (struct buffer){0};
	return V(type_buf,&clear,(value)buf);
	(void)f;
	}

// (buf_put buf str) Appends the string to the buffer.
value type_buf_put(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_buf && y->T == type_str)
		{
		buf_put(get_buf(x),get_str(y));
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

// (buf_get buf) Clear the buffer and return str, where str is the current
// content of the buffer.
value type_buf_get(value f)
	{
	value x = arg(f->R);
	if (x->T == type_buf)
		f = Qstr(buf_clear(get_buf(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
