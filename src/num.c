#include <math.h> /* pow fabs sqrt exp log sin cos M_PI */
#include <memory.h>
#include <num.h>

/* Explicitly declare certain functions which are missing from math.h on some
machines. */
extern double round(double);
extern double trunc(double);

void num_free(number x)
	{
	free_memory(x,sizeof(double));
	}

number num_new_double(double val)
	{
	number p = new_memory(sizeof(double));
	*p = val;
	return p;
	}

number num_new_ulong(unsigned long val)
	{
	return num_new_double((double)val);
	}

number num_add(number x, number y) { return num_new_double(*x + *y); }
number num_sub(number x, number y) { return num_new_double(*x - *y); }
number num_mul(number x, number y) { return num_new_double(*x * *y); }
number num_div(number x, number y) { return num_new_double(*x / *y); }
number num_pow(number x, number y) { return num_new_double(pow(*x,*y)); }
number num_xor(number x, number y)
	{
	return num_new_double((long)*x ^ (long)*y);
	}

number num_round(number x) { return num_new_double(round(*x)); }
number num_trunc(number x) { return num_new_double(trunc(*x)); }
number num_abs(number x) { return num_new_double(fabs(*x)); }
number num_sqrt(number x) { return num_new_double(sqrt(*x)); }
number num_exp(number x) { return num_new_double(exp(*x)); }
number num_log(number x) { return num_new_double(log(*x)); }
number num_sin(number x) { return num_new_double(sin(*x)); }
number num_cos(number x) { return num_new_double(cos(*x)); }

number num_pi(void) { return num_new_double(M_PI); }

int num_cmp(number x, number y)
	{
	if (*x < *y) return -1;
	if (*x > *y) return 1;
	return 0;
	}
