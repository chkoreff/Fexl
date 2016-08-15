#include <str.h>
#include <value.h>
#include <basic.h>
#include <input.h>
#include <type_input.h>
#include <type_str.h>

value op_getc(value f, type t, input get)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == t)
		{
		int ch = get(data(x));
		if (ch == -1)
			f = Qvoid();
		else
			{
			char c = (char)ch;
			f = Qstr(str_new_data(&c,1));
			}
		action = 1;
		f = yield(f);
		}
	else
		reduce_void(f);
	drop(x);
	return f;
	}
	}

value op_get(value f, type t, input get)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == t)
		{
		string ch = get_utf8(get,data(x));
		action = 1;
		f = yield(ch ? Qstr(ch) : Qvoid());
		}
	else
		reduce_void(f);
	drop(x);
	return f;
	}
	}
