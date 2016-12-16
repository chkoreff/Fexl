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
	return type_void(f);
	}

value Qstr(string x)
	{
	return D(type_str,x,(type)str_free);
	}

value reduce_str(value f, string x)
	{
	return reduce_D(f,type_str,x,(type)str_free);
	}

/* (. x y) is the concatenation of strings x and y. */
value type_concat(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		f = reduce_str(f,str_concat(data(x),data(y)));
	else
		f = reduce_void(f);
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
		f = reduce_num(f,num_new_ulong(((string)data(x))->len));
	else
		f = reduce_void(f);
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
		double yn = *((number)data(y));
		double zn = *((number)data(z));
		if (yn >= 0 && zn >= 0)
			f = reduce_str(f,str_slice(data(x),yn,zn));
		else
			f = reduce_void(f);
		}
	else
		f = reduce_void(f);
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
		double zn = *((number)data(z));
		if (zn >= 0)
			{
			string xs = data(x);
			string ys = data(y);
			unsigned long pos = str_search(xs,ys,zn);
			if (pos < xs->len)
				f = reduce_num(f,num_new_ulong(pos));
			else
				f = reduce_void(f);
			}
		else
			f = reduce_void(f);
		}
	else
		f = reduce_void(f);
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
		number n = str0_num(((string)data(x))->data);
		if (n)
			f = reduce_num(f,n);
		else
			f = reduce_void(f);
		}
	else
		f = reduce_void(f);
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
		string xs = data(x);
		f = reduce_num(f,num_new_ulong(xs->len == 0 ? 0 :
			(unsigned char)xs->data[0]));
		}
	else
		f = reduce_void(f);
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
		double xn = *((number)data(x));
		char ch = xn;
		f = reduce_str(f,str_new_data(&ch,1));
		}
	else
		f = reduce_void(f);
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
		double yn = *((number)data(y));
		if (yn >= 0)
			{
			string xs = data(x);
			unsigned long pos = yn;
			if (pos < xs->len)
				{
				char n = char_width(xs->data[pos]);
				f = reduce_num(f,num_new_ulong(n));
				}
			else
				f = reduce_void(f);
			}
		else
			f = reduce_void(f);
		}
	else
		f = reduce_void(f);
	drop(x);
	drop(y);
	return f;
	}
	}

value type_is_str(value f)
	{
	return op_is_type(f,type_str);
	}
