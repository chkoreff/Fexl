#include <str.h>

#include <input.h>
#include <value.h>

#include <basic.h>
#include <type_input.h>
#include <type_str.h>

value op_getc(value fun, value f, type t, input get)
	{
	value x = arg(f->R);
	if (x->T == t)
		{
		void *source = x->v_ptr;
		int ch = get(source);
		if (ch == -1)
			f = hold(Qvoid);
		else
			{
			char c = (char)ch;
			f = Qstr(str_new_data(&c,1));
			}
		}
	else
		f = hold(Qvoid);
	drop(x);
	(void)fun;
	return f;
	}

value op_get(value fun, value f, type t, input get)
	{
	value x = arg(f->R);
	if (x->T == t)
		{
		void *source = x->v_ptr;
		string ch = get_utf8(get,source);
		f = ch ? Qstr(ch) : hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	(void)fun;
	return f;
	}
