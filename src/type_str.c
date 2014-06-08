#include <value.h>

#include <num.h>
#include <str.h>
#include <type_num.h>
#include <type_str.h>

value type_str(value f)
	{
	if (!f->N) str_free((string)f->R);
	return 0;
	}

string get_str(value f)
	{
	return (string)get_data(f,type_str);
	}

value Qstr(string p)
	{
	return D(type_str,p);
	}

value Qstr0(const char *data)
	{
	return Qstr(str_new_data0(data));
	}

/* (. x y) is the concatenation of strings x and y. */
value type_concat(value f)
	{
	value x, y, z;
	if (!f->L || !f->L->L) return 0;
	x = arg(f->L->R);
	y = arg(f->R);
	z = Qstr(str_concat(get_str(x),get_str(y)));
	drop(x);
	drop(y);
	return z;
	}

/* (length x) is the length of string x */
value type_length(value f)
	{
	value x, y;
	if (!f->L) return 0;
	x = arg(f->R);
	{
	unsigned long len = get_str(x)->len;
	y = Qnum_ulong(len);
	}
	drop(x);
	return y;
	}

/*TODO 20140607 this is not aware of UTF-8.  For that, we should probably use
an "utf8_chars" function which splits a string into a list of logical
characters. */
value type_slice(value f)
	{
	value x, y, z, result;
	string str;
	long pos, len;

	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	x = arg(f->L->L->R);
	y = arg(f->L->R);
	z = arg(f->R);
	str = get_str(x);
	pos = (long)*get_num(y);
	len = (long)*get_num(z);

	if (pos < 0 || len < 0
		|| pos >= str->len
		|| len > str->len
		|| pos > str->len - len
		)
		{
		pos = 0;
		len = 0;
		}

	result = Qstr(str_new_data(str->data + pos,len));
	drop(x);
	drop(y);
	drop(z);
	return result;
	}
