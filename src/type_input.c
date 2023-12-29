#include <str.h>
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
	return f;
	(void)fun;
	}

// Get the next UTF-8 character from the given input, or 0 if none.
static string get_utf8(input get, void *source)
	{
	int ch = get(source);
	if (ch == -1) return 0;
	{
	char buf[6];
	unsigned pos = 0;
	unsigned len = char_width(ch);

	if (len >= sizeof(buf))
		len = sizeof(buf);

	while (1)
		{
		buf[pos++] = (char)ch;
		if (pos >= len) break;
		ch = get(source);
		if (ch == -1) return 0;
		}
	return str_new_data(buf,len);
	}
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
	return f;
	(void)fun;
	}
