#include <num.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <convert.h>
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

void reduce_str(value f, string x)
	{
	reduce_D(f,type_str,x,(type)str_free);
	}

/* (. x y) is the concatenation of strings x and y. */
value type_concat(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		reduce_str(f, str_concat(data(x),data(y)));
	else
		reduce_void(f);
	return 0;
	}
	}

/* (length x) is the length of string x */
value type_length(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		reduce_num(f, num_new_ulong(((string)data(x))->len));
	else
		reduce_void(f);
	return 0;
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
			{
			reduce_str(f, str_slice(data(x),yn,zn));
			return 0;
			}
		}
	reduce_void(f);
	return 0;
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
				{
				reduce_num(f, num_new_ulong(pos));
				return 0;
				}
			}
		}
	reduce_void(f);
	return 0;
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
			{
			reduce_num(f,n);
			return 0;
			}
		}
	reduce_void(f);
	return 0;
	}
	}

value type_is_str(value f)
	{
	return op_is_type(f,type_str);
	}
