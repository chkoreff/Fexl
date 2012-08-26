#include "value.h"
#include "basic.h"
#include "double.h"
#include "long.h"
#include "string.h"

void type_long(value f) { type_error(); }

/* Make a new value of type long.  */
value Qlong(long number)
	{
	value f = D(type_long);
	f->R->L = (value)number;
	return f;
	}

long get_long(value f)
	{
	return (long)f->R->L;
	}

static void reduce2_long_add(value f)
	{
	long x = get_long(arg(type_long,f->L->R));
	long y = get_long(arg(type_long,f->R));
	replace(f, Qlong(x + y));
	}

void reduce_long_add(value f)
	{
	f->T = reduce2_long_add;
	}

static void reduce2_long_sub(value f)
	{
	long x = get_long(arg(type_long,f->L->R));
	long y = get_long(arg(type_long,f->R));
	replace(f, Qlong(x - y));
	}

void reduce_long_sub(value f)
	{
	f->T = reduce2_long_sub;
	}

static void reduce2_long_mul(value f)
	{
	long x = get_long(arg(type_long,f->L->R));
	long y = get_long(arg(type_long,f->R));
	replace(f, Qlong(x * y));
	}

void reduce_long_mul(value f)
	{
	f->T = reduce2_long_mul;
	}

/* long_div returns 0 if you try to divide by 0. */
static void reduce2_long_div(value f)
	{
	long x = get_long(arg(type_long,f->L->R));
	long y = get_long(arg(type_long,f->R));
	replace(f, Qlong(y ? x / y : 0));
	}

void reduce_long_div(value f)
	{
	f->T = reduce2_long_div;
	}

/* Determine if the value has type long. */
void reduce_is_long(value f)
	{
	replace_boolean(f, if_type(type_long,f->R));
	}

/* Convert long to double. */
void reduce_long_double(value f)
	{
	long x = get_long(arg(type_long,f->R));
	replace(f, Qdouble(x));
	}

/* Convert long to string. */
extern int sprintf(char *str, const char *format, ...);
void reduce_long_string(value f)
	{
	long x = get_long(arg(type_long,f->R));
	char buf[100]; /* being careful here */
	sprintf(buf, "%ld", x);
	replace(f, Qcopy_string(buf));
	}

/* Convert long to char (string of length 1). */
void reduce_long_char(value f)
	{
	long x = get_long(arg(type_long,f->R));
	char buf[1];
	buf[0] = x;
	replace(f, Qcopy_chars(buf,1));
	}

/* (long_cmp x y) compares x and y and returns <0, 0, or >0. */
static void reduce2_long_cmp(value f)
	{
	long x = get_long(arg(type_long,f->L->R));
	long y = get_long(arg(type_long,f->R));
	replace(f, Qlong(x < y ? -1 : x > y ? 1 : 0));
	}

void reduce_long_cmp(value f)
	{
	f->T = reduce2_long_cmp;
	}

/* (order x lt eq gt) compares x and 0 and returns lt, eq, or gt. */
static void reduce4_order(value f)
	{
	long x = get_long(arg(type_long,f->L->L->L->R));
	replace(f, x < 0 ? f->L->L->R : x > 0 ? f->R : f->L->R);
	}

static void reduce3_order(value f)
	{
	f->T = reduce4_order;
	}

static void reduce2_order(value f)
	{
	f->T = reduce3_order;
	}

void reduce_order(value f)
	{
	f->T = reduce2_order;
	}
