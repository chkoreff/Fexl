#include <stdio.h> /* sprintf */
#include "value.h"
#include "basic.h"
#include "double.h"
#include "long.h"
#include "stack.h"
#include "string.h"

struct value *type_long(struct value *f) { return f; }

/* Make a new value of type long.  */
struct value *Qlong(long number)
	{
	struct value *atom = create();
	atom->N = 1;
	atom->T = type_long;
	atom->L = (struct value *)number;

	struct value *value = create();
	value->N = 0;
	value->T = type_long;
	value->L = 0;
	value->R = atom;

	return value;
	}

long get_long(struct value *f)
	{
	return (long)f->R->L;
	}

struct value *type_long_add(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *x = f->L->R;
	struct value *y = f->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	return Qlong(get_long(x) + get_long(y));
	}

struct value *type_long_sub(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *x = f->L->R;
	struct value *y = f->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	return Qlong(get_long(x) - get_long(y));
	}

struct value *type_long_mul(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *x = f->L->R;
	struct value *y = f->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	return Qlong(get_long(x) * get_long(y));
	}

/* long_div returns 0 if you try to divide by 0. */
struct value *type_long_div(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *x = f->L->R;
	struct value *y = f->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	long vx = get_long(x);
	long vy = get_long(y);

	return Qlong(vy ? vx / vy : 0);
	}

/*TODO perhaps also a same_type function for checking types at run-time */

/* Determine if the value has type long.
TODO perhaps don't refer to C and F.
TODO perhaps rename type_long_type
*/
struct value *type_is_long(struct value *f)
	{
	struct value *x = f->R;
	if (arg(type_long,x)) return C;
	if (x->T == 0) return f;
	return F;
	}

/* Convert long to double. */
struct value *type_long_double(struct value *f)
	{
	struct value *x = f->R;
	if (!arg(type_long,x)) return f;

	return Qdouble(get_long(x));
	}

struct value *type_long_string(struct value *f)
	{
	struct value *x = f->R;
	if (!arg(type_long,x)) return f;

	char buf[100]; /* being careful here */
	sprintf(buf, "%ld", get_long(x));
	return Qcopy_string(buf);
	}

/* Convert long to char (string of length 1). */
struct value *type_long_char(struct value *f)
	{
	struct value *x = f->R;
	if (!arg(type_long,x)) return f;

	char buf[1];
	buf[0] = get_long(x);
	return Qcopy_chars(buf,1);
	}

/* long_compare x y lt eq gt */
struct value *type_long_compare(struct value *f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L || !f->L->L->L->L->L)
		return f;

	struct value *x = f->L->L->L->L->R;
	struct value *y = f->L->L->L->R;

	if (!arg(type_long,x)) return f;
	if (!arg(type_long,y)) return f;

	long vx = get_long(x);
	long vy = get_long(y);

	if (vx < vy) return f->L->L->R;
	if (vx > vy) return f->R;
	return f->L->R;
	}
