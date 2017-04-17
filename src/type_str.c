#include <num.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <convert.h>
#include <input.h>
#include <type_num.h>
#include <type_str.h>

value type_str(value f)
	{
	if (f->N == 0)
		{
		str_free(get_str(f));
		return 0;
		}
	return type_void(f);
	}

value Qstr(string x)
	{
	return D(type_str,x);
	}

value Qstr0(const char *data)
	{
	return Qstr(str_new_data0(data));
	}

string get_str(value x)
	{
	return (string)x->R;
	}

const char *str_data(value x)
	{
	return get_str(x)->data;
	}

/* (. x y) is the concatenation of strings x and y. */
value type_concat(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		f = Qstr(str_concat(get_str(x),get_str(y)));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (length x) is the length of string x */
value type_length(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qnum(num_new_ulong(get_str(x)->len));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (slice str pos len) calls str_slice, except it returns void if pos or len is
negative. */
value type_slice(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	value y = arg(f->L->R);
	value z = arg(f->R);
	if (x->T == type_str && y->T == type_num && z->T == type_num)
		{
		double yn = get_double(y);
		double zn = get_double(z);
		if (yn >= 0 && zn >= 0)
			f = Qstr(str_slice(get_str(x),yn,zn));
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	drop(z);
	return f;
	}
	}

/* (search haystack needle offset) calls str_search. */
value type_search(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	value y = arg(f->L->R);
	value z = arg(f->R);
	if (x->T == type_str && y->T == type_str && z->T == type_num)
		{
		double zn = get_double(z);
		if (zn >= 0)
			{
			string xs = get_str(x);
			string ys = get_str(y);
			unsigned long pos = str_search(xs,ys,zn);
			if (pos < xs->len)
				f = Qnum(num_new_ulong(pos));
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	drop(z);
	return f;
	}
	}

/* Convert string to number if possible. */
value type_str_num(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		number n = str0_num(str_data(x));
		if (n)
			f = Qnum(n);
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (ord x) is the ordinal number of the first ASCII character of string x. */
value type_ord(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		string xs = get_str(x);
		f = Qnum(num_new_ulong(xs->len == 0 ? 0 :
			(unsigned char)xs->data[0]));
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (chr x) is the ASCII character whose ordinal number is x. */
value type_chr(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		double xn = get_double(x);
		char ch = xn;
		f = Qstr(str_new_data(&ch,1));
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (char_width str pos) Return the width of the UTF-8 character which starts at
the given position. */
value type_char_width(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_num)
		{
		double yn = get_double(y);
		if (yn >= 0)
			{
			string xs = get_str(x);
			unsigned long pos = yn;
			if (pos < xs->len)
				{
				char n = char_width(xs->data[pos]);
				f = Qnum(num_new_ulong(n));
				}
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

value type_is_str(value f)
	{
	return op_is_type(f,type_str);
	}
