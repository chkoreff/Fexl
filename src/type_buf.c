#include <str.h>
#include <value.h>

#include <basic.h>
#include <buffer.h>
#include <type_buf.h>
#include <type_str.h>

buffer get_buf(value x)
	{
	return x->v_ptr;
	}

value type_buf(value f)
	{
	return type_atom(f);
	}

static void clear_buf(value f)
	{
	buf_free(f->v_ptr);
	}

/* buf_new returns a new empty character buffer. */
value type_buf_new(value f)
	{
	static struct value atom = {{.N=0}, {.clear=clear_buf}};
	(void)f;
	return V(type_buf,&atom,(value)buf_new());
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

/* (buf_get buf) Clear the buffer and return str, where str is the current
content of the buffer. */
value type_buf_get(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_buf)
		f = Qstr(buf_clear(get_buf(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}
