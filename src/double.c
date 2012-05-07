#include "value.h"
#include "double.h"
#include "long.h"
#include "string.h"

value type_double(value f) { return 0; }

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

value fexl_double_add(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_double,x)) return 0;
	if (!arg(type_double,y)) return 0;

	return Qdouble(get_double(x) + get_double(y));
	}

value fexl_double_sub(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_double,x)) return 0;
	if (!arg(type_double,y)) return 0;

	return Qdouble(get_double(x) - get_double(y));
	}

value fexl_double_mul(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_double,x)) return 0;
	if (!arg(type_double,y)) return 0;

	return Qdouble(get_double(x) * get_double(y));
	}

/*
Note that dividing by zero is no problem here.  If you divide a non-zero by
zero, it yields inf (infinity).  If you divide zero by zero, it yields -nan
(not a number).
*/
value fexl_double_div(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_double,x)) return 0;
	if (!arg(type_double,y)) return 0;

	return Qdouble(get_double(x) / get_double(y));
	}

/* Determine if the value has type double. */
value fexl_is_double(value f)
	{
	return arg_is_type(type_double,f);
	}

/* double_string num */
/*LATER note that sprintf "%.15g" shows 0.0 as "0", which looks like a long if
we try to read it back in.  Perhaps force a ".0" suffix, not sure. */
extern int sprintf(char *str, const char *format, ...);
value fexl_double_string(value f)
	{
	if (!f->L) return 0;

	value x = f->R;
	if (!arg(type_double,x)) return 0;

	char buf[100]; /* being careful here */
	/* We show 15 digits because that's what Perl does. */
	sprintf(buf, "%.15g", get_double(x));
	return Qcopy_string(buf);
	}

value fexl_double_long(value f)
	{
	if (!f->L) return 0;

	value x = f->R;
	if (!arg(type_double,x)) return 0;

	return Qlong(get_double(x));
	}

/* double_compare x y lt eq gt */
value fexl_double_compare(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L
		|| !f->L->L->L->L->L)
		return 0;

	value x = f->L->L->L->L->R;
	value y = f->L->L->L->R;

	if (!arg(type_double,x)) return 0;
	if (!arg(type_double,y)) return 0;

	double vx = get_double(x);
	double vy = get_double(y);

	if (vx < vy) return f->L->L->R;
	if (vx > vy) return f->R;
	return f->L->R;
	}
