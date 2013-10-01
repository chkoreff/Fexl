#include "memory.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "long.h"
#include "qstr.h"

/*LATER wrap a combinator around the buf.c module, which will allow very fast
large-scale buffering */

value type_string(value f)
	{
	if (!f->N) str_free(as_str(f));
	return f;
	}

value Qstr(struct str *p)
	{
	return V(type_string,0,(value)p);
	}

value Qstr0(const char *data)
	{
	return Qstr(str_new_data0(data));
	}

struct str *as_str(value f)
	{
	return (struct str *)f->R;
	}

struct str *get_str(value f)
	{
	if (f->T != type_string) bad_type();
	return as_str(f);
	}

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

/* (string_append x y) is the concanation of strings x and y. */
value type_string_append(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qstr(str_append(get_str(x),get_str(y)));
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

/*TODO move some of these to a separate library? */

#if 0
TODO more functions
string_slice str pos len
string_index
string_long
string_double
#endif
