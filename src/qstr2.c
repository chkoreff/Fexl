#include "str.h"

#include "value.h"
#include "long.h"
#include "qstr.h"

/* (string_at str pos) is the character at pos, or 0 if out of bounds. */
value type_string_at(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qlong(str_at(get_str(x),get_long(y)));
	drop(x);
	drop(y);
	return z;
	}

/* (string_cmp x y) compares x and y and returns <0, 0, or >0. */
value type_string_cmp(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qlong(str_cmp(get_str(x),get_str(y)));
	drop(x);
	drop(y);
	return z;
	}

/* (string_len x) is the length of string x */
value type_string_len(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	value z = Qlong(get_str(x)->len);
	drop(x);
	return z;
	}

/* Compute the length of the longest common prefix of two strings. */
value type_string_common(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qlong(str_common(get_str(x),get_str(y)));
	drop(x);
	drop(y);
	return z;
	}

#if 0
LATER more functions
string_slice str pos len
string_index
string_long
string_double
#endif
