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
	if (!f->L || !f->L->L) return 0;
	value x = arg(f->L->R);
	value y = arg(f->R);
	value z = Qstr(str_concat(get_str(x),get_str(y)));
	drop(x);
	drop(y);
	return z;
	}

/* (length x) is the length of string x */
value type_length(value f)
	{
	if (!f->L) return 0;
	value x = arg(f->R);
	unsigned long len = get_str(x)->len;
	value z = Qnum_ulong(len);
	drop(x);
	return z;
	}
