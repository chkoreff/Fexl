#include <stdio.h> /* sprintf */
#include "value.h"
#include "double.h"
#include "long.h"
#include "stack.h"
#include "string.h"

value type_long(value f) { return f; }

/* Make a new value of type long.  */
value Qlong(long number)
	{
	value atom = create();
	atom->N = 1;
	atom->T = type_long;
	atom->L = (value)number;

	value f = create();
	f->N = 0;
	f->T = type_long;
	f->L = 0;
	f->R = atom;

	return f;
	}

long get_long(value f)
	{
	return (long)f->R->L;
	}

value type_long_add(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	return Qlong(get_long(x) + get_long(y));
	}

value type_long_sub(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	return Qlong(get_long(x) - get_long(y));
	}

value type_long_mul(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	return Qlong(get_long(x) * get_long(y));
	}

/* long_div returns 0 if you try to divide by 0. */
value type_long_div(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	long vx = get_long(x);
	long vy = get_long(y);

	return Qlong(vy ? vx / vy : 0);
	}

/* Determine if the value has type long. */
value type_is_long(value f)
	{
	return arg_is_type(type_long,f);
	}

/* Convert long to double. */
value type_long_double(value f)
	{
	value x = f->R;
	if (!arg(type_long,x)) return f;

	return Qdouble(get_long(x));
	}

value type_long_string(value f)
	{
	value x = f->R;
	if (!arg(type_long,x)) return f;

	char buf[100]; /* being careful here */
	sprintf(buf, "%ld", get_long(x));
	return Qcopy_string(buf);
	}

/* Convert long to char (string of length 1). */
value type_long_char(value f)
	{
	value x = f->R;
	if (!arg(type_long,x)) return f;

	char buf[1];
	buf[0] = get_long(x);
	return Qcopy_chars(buf,1);
	}

/* long_compare x y lt eq gt */
value type_long_compare(value f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L || !f->L->L->L->L->L)
		return f;

	value x = f->L->L->L->L->R;
	value y = f->L->L->L->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	long vx = get_long(x);
	long vy = get_long(y);

	if (vx < vy) return f->L->L->R;
	if (vx > vy) return f->R;
	return f->L->R;
	}
