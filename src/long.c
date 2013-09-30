#include <stdlib.h> /* strtol */
#include "value.h"
#include "basic.h"
#include "long.h"

value type_long(value f)
	{
	return f;
	}

value Qlong(long val)
	{
	return V(type_long,0,(value)val);
	}

long get_long(value f)
	{
	if (f->T != type_long) bad_type();
	return (long)f->R;
	}

static long op_add(long x, long y) { return x + y; }
static long op_sub(long x, long y) { return x - y; }
static long op_mul(long x, long y) { return x * y; }
/* op_div returns 0 if you try to divide by 0. */
static long op_div(long x, long y) { return y ? x / y : 0; }

static value op2(value f, long op(long,long))
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qlong(op(get_long(x),get_long(y)));
	drop(x);
	drop(y);
	return z;
	}

value type_long_add(value f) { return op2(f,op_add); }
value type_long_sub(value f) { return op2(f,op_sub); }
value type_long_mul(value f) { return op2(f,op_mul); }
value type_long_div(value f) { return op2(f,op_div); }

/* Convert string to long and return true if successful. */
int string_long(const char *beg, long *num)
	{
	char *end;
	*num = strtol(beg, &end, 10);
	return *beg != '\0' && *end == '\0';
	}

/*TODO more functions */
