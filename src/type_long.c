#include <value.h>
#include <stdlib.h> /* strtol */
#include <str.h>
#include <type_double.h>
#include <type_long.h>
#include <type_str.h>

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
	return (long)get_data(f,type_long);
	}

/* Convert string to long and return true if successful. */
int string_long(const char *beg, long *num)
	{
	char *end;
	*num = strtol(beg, &end, 10);
	return *beg != '\0' && *end == '\0';
	}

static long op_add(long x, long y) { return x + y; }
static long op_sub(long x, long y) { return x - y; }
static long op_mul(long x, long y) { return x * y; }
/* op_div returns 0 if you try to divide by 0. */
static long op_div(long x, long y) { return y ? x / y : 0; }

static long op_cmp(long x, long y) { return x < y ? -1 : x > y ? 1 : 0; }

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

/* (long_cmp x y) compares x and y and returns -1, 0, or +1. */
value type_long_cmp(value f) { return op2(f,op_cmp); }

/* Convert long to string. */
extern int sprintf(char *str, const char *format, ...);
value type_long_string(value f)
	{
	if (!f->L) return f;

	value x = eval(f->R);
	char buf[100]; /* being careful here */
	sprintf(buf, "%ld", get_long(x));

	value y = Qstr0(buf);
	drop(x);
	return y;
	}

/* (order x lt eq gt) compares x and 0 and returns lt, eq, or gt.  This allows
three-way branching on the sign of a long value, typically the result of a
comparison function. */
value type_order(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return f;
	value xv = eval(f->L->L->L->R);
	long x = get_long(xv);
	value g = x < 0 ? f->L->L->R : x > 0 ? f->R : f->L->R;
	drop(xv);
	return g;
	}

value type_long_double(value f) /*TODO*/
	{
	if (!f->L) return f;
	value x = eval(f->R);
	value y = Qdouble(get_long(x));
	drop(x);
	return y;
	}

/* Convert long to char (string of length 1). */
value type_long_char(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	char buf[1];
	buf[0] = get_long(x);
	value result = Qstr(str_new_data(buf,1));
	drop(x);
	return result;
	}
