#include <num.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <convert.h>
#include <type_num.h>
#include <type_str.h>

value type_str(value f)
	{
	if (f->N == 0) str_free((string)f->R);
	if (!f->L) return f;
	return Q(type_void);
	}

value Qstr(string x)
	{
	return D(type_str,x);
	}

/* (. x y) is the concatenation of strings x and y. */
value type_concat(value f)
	{
	if (!f->L || !f->L->L) return f;
	{
	value x = eval(hold(f->L->R));
	value y = eval(hold(f->R));
	if (x->T == type_str && y->T == type_str)
		f = Qstr(str_concat((string)x->R,(string)y->R));
	else
		f = Q(type_void);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (length x) is the length of string x */
value type_length(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		f = Qnum(num_new_ulong(((string)x->R)->len));
	else
		f = Q(type_void);
	drop(x);
	return f;
	}
	}

/* LATER str_find */

/* (slice str pos len) returns the len bytes of str starting at pos, or void if
pos or len exceeds the bounds of str. */
value type_slice(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	{
	value x = eval(hold(f->L->L->R));
	value y = eval(hold(f->L->R));
	value z = eval(hold(f->R));
	f = 0;
	if (x->T == type_str && y->T == type_num && z->T == type_num)
		{
		double yn = *((number)y->R);
		double zn = *((number)z->R);
		if (yn >= 0 && zn >= 0)
			{
			string xs = (string)x->R;
			unsigned long pos = (unsigned long)yn;
			unsigned long len = (unsigned long)zn;
			if (pos < xs->len
				&& len <= xs->len
				&& pos <= xs->len - len)
				f = Qstr(str_new_data(xs->data + pos,len));
			}
		}
	drop(x);
	drop(y);
	drop(z);
	if (!f) f = Q(type_void);
	return f;
	}
	}

/* Convert string to number if possible. */
value type_str_num(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	f = 0;
	if (x->T == type_str)
		{
		number n = str0_num(((string)x->R)->data);
		if (n)
			f = Qnum(n);
		}
	drop(x);
	if (!f) f = Q(type_void);
	return f;
	}
	}

value type_is_str(value f)
	{
	return Qis_atom(type_str,f);
	}
