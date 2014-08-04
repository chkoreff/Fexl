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
	return type_bad(f);
	}

value Qstr(string x)
	{
	if (!x) return 0;
	{
	value f = D(type_str,x);
	if (!f) str_free(x);
	return f;
	}
	}

/* (. x y) is the concatenation of strings x and y. */
value type_concat(value f)
	{
	if (!f->L || !f->L->L) return f;
	{
	value x = eval(hold(f->L->R));
	value y = eval(hold(f->R));
	value g;
	if (is_atom(type_str,x) && is_atom(type_str,y))
		g = Qstr(str_concat((string)x->R,(string)y->R));
	else
		g = hold(bad);
	drop(x);
	drop(y);
	return g;
	}
	}

/* (length x) is the length of string x */
value type_length(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	value g;
	if (is_atom(type_str,x))
		g = Qnum(num_new_ulong(((string)x->R)->len));
	else
		g = hold(bad);
	drop(x);
	return g;
	}
	}

/* (slice str pos len) returns the len bytes of str starting at pos, or bad if
pos or len exceeds the bounds of str. */
value type_slice(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	{
	value x = eval(hold(f->L->L->R));
	value y = eval(hold(f->L->R));
	value z = eval(hold(f->R));
	value g = F;
	if (is_atom(type_str,x) && is_atom(type_num,y) && is_atom(type_num,z))
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
				g = Qstr(str_new_data(xs->data + pos,len));
			}
		}
	if (g == F) g = hold(bad);
	drop(x);
	drop(y);
	drop(z);
	return g;
	}
	}

/* Convert string to number if possible. */
value type_str_num(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	value g = F;
	if (is_atom(type_str,x))
		{
		int ok;
		number num = str0_num(((string)x->R)->data,&ok);
		if (ok) g = Qnum(num);
		}
	if (g == F) g = hold(bad);
	drop(x);
	return g;
	}
	}

value type_is_str(value f)
	{
	return Qis_type(type_str,f);
	}
