#include <num.h>
#include <output.h>

#include <die.h>
#include <format.h>
#include <memory.h>
#include <stdlib.h> /* strtod */

/* NOTE: On some machines, when compiling in -ansi mode, I can't simply include
math.h to get certain built-in functions, because they're declared somewhere
else as returning type int.  So instead I do my own declarations. */
extern double round(double);
extern double trunc(double);
extern double fabs(double);
extern double pow(double,double);

void num_free(number x)
	{
	free_memory(x, sizeof(double));
	}

/* Convert string to number if possible. */
/* LATER prevent too much precision -- in other words, guarantee exact
representation on any machine. */
number str_num(const char *name)
	{
	char *end;
	double val;
	if (*name == 0) return 0;
	val = strtod(name, &end);
	if (*end != '\0') return 0;
	return num_new_double(val);
	}

number num_new_double(double val)
	{
	number p = new_memory(sizeof(double));
	*p = val;
	return p;
	}

number num_new_ulong(unsigned long val)
	{
	double x = (double)val;
	/* Verify that we can convert x back to same val. */
	if (val != (long)x) die("num_new_long");
	return num_new_double(x);
	}

number num_add(number x, number y) { return num_new_double(*x + *y); }
number num_sub(number x, number y) { return num_new_double(*x - *y); }
number num_mul(number x, number y) { return num_new_double(*x * *y); }
number num_div(number x, number y) { return num_new_double(*x / *y); }
number num_pow(number x, number y) { return num_new_double(pow(*x,*y)); }
number num_round(number x) { return num_new_double(round(*x)); }
number num_trunc(number x) { return num_new_double(trunc(*x)); }
number num_abs(number x) { return num_new_double(fabs(*x)); }

int num_cmp(number x, number y)
	{
	if (*x < *y) return -1;
	if (*x > *y) return 1;
	return 0;
	}

/* We show 15 digits because that's what Perl does. */

/* https://en.wikipedia.org/wiki/Double-precision_floating-point_format */
/* LATER use limits to determine precision for machine */
/* LATER problem is, DECIMAL_DIG (in float.h) is only guaranteed to be at least
10. */
void put_num(number x)
	{
	put_double(*x);
	}

const char *format_num(number x)
	{
	return format_double(*x);
	}
