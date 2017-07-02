#include <str.h>

#include <input.h>
#include <value.h>

#include <standard.h>
#include <type_input.h>
#include <type_str.h>

value op_getc(value f, type t, input get)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == t)
		{
		void *source = x->R;
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
		void *source = x->R;
		string ch = get_utf8(get,source);
		f = ch ? Qstr(ch) : hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}
