#include "value.h"
#include "basic.h"
#include "double.h"
#include "long.h"
#include "string.h"

/*LATER ordinary built-in srand and rand routines */

void type_double(value f) { type_error(); }

/*
Make a new value of type double.

A double is guaranteed to fit in 64 bits.  The standard says:

  Type        Range: Min   Max           Size (Bits)
  double      2.225e-308   1.7977e+308   64

(C Language Reference Manual, document 007-0701-150, Appendix A, Section F.3.6
"Floating Point")

We know that the L and R pointers are either 32 or 64 bits each, depending on
the machine, so together they can definitely hold the 64 bits of a double.
*/
value Qdouble(double number)
	{
	value f = D(type_double);
	double *p = (double *)(void *)(&f->R->L);
	*p = number;
	return f;
	}

/*
Note that we cannot retrieve a double value directly like this:
	double x = *( (double *)&f->R->L );

That yields an error:
	dereferencing type-punned pointer will break strict-aliasing rules
	[-Werror=strict-aliasing]

Instead, we must first get a pointer to double, then dereference the pointer.
*/
double get_double(value f)
	{
	double *x = (double *)(void *)&f->R->L;
	return *x;
	}

static void reduce2_double_add(value f)
	{
	double x = get_double(arg(type_double,f->L->R));
	double y = get_double(arg(type_double,f->R));
	replace(f, Qdouble(x + y));
	}

void reduce_double_add(value f)
	{
	f->T = reduce2_double_add;
	}

static void reduce2_double_sub(value f)
	{
	double x = get_double(arg(type_double,f->L->R));
	double y = get_double(arg(type_double,f->R));
	replace(f, Qdouble(x - y));
	}

void reduce_double_sub(value f)
	{
	f->T = reduce2_double_sub;
	}

static void reduce2_double_mul(value f)
	{
	double x = get_double(arg(type_double,f->L->R));
	double y = get_double(arg(type_double,f->R));
	replace(f, Qdouble(x * y));
	}

void reduce_double_mul(value f)
	{
	f->T = reduce2_double_mul;
	}

/*
Note that dividing by zero is no problem here.  If you divide a non-zero by
zero, it yields inf (infinity).  If you divide zero by zero, it yields -nan
(not a number).
*/
static void reduce2_double_div(value f)
	{
	double x = get_double(arg(type_double,f->L->R));
	double y = get_double(arg(type_double,f->R));
	replace(f, Qdouble(x / y));
	}

void reduce_double_div(value f)
	{
	f->T = reduce2_double_div;
	}

/* Determine if the value has type double. */
void reduce_is_double(value f)
	{
	replace_boolean(f, if_type(type_double,f->R));
	}

/* double_string num */
/*LATER note that sprintf "%.15g" shows 0.0 as "0", which looks like a long if
we try to read it back in.  Perhaps force a ".0" suffix, not sure. */
extern int sprintf(char *str, const char *format, ...);
void reduce_double_string(value f)
	{
	double x = get_double(arg(type_double,f->R));
	char buf[100]; /* being careful here */
	/* We show 15 digits because that's what Perl does. */
	sprintf(buf, "%.15g", x);
	replace(f, Qcopy_string(buf));
	}

void reduce_double_long(value f)
	{
	double x = get_double(arg(type_double,f->R));
	replace(f, Qlong(x));
	}

/* (double_cmp x y) compares x and y and returns <0, 0, or >0. */
static void reduce2_double_cmp(value f)
	{
	double x = get_double(arg(type_double,f->L->R));
	double y = get_double(arg(type_double,f->R));
	replace(f, Qlong(x < y ? -1 : x > y ? 1 : 0));
	}

void reduce_double_cmp(value f)
	{
	f->T = reduce2_double_cmp;
	}
