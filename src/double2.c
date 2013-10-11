#include "str.h"

#include "value.h"
#include "double.h"
#include "qstr.h"

static double op_add(double x, double y) { return x + y; }
static double op_sub(double x, double y) { return x - y; }
static double op_mul(double x, double y) { return x * y; }
/*
Note that dividing by zero is no problem here. If you divide a non-zero by
zero, it yields inf (infinity). If you divide zero by zero, it yields -nan
(not a number).
*/
static double op_div(double x, double y) { return x / y; }

static double op_cmp(double x, double y) { return x < y ? -1 : x > y ? 1 : 0; }

static value op2(value f, double op(double,double))
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qdouble(op(get_double(x),get_double(y)));
	drop(x);
	drop(y);
	return z;
	}

value type_double_add(value f) { return op2(f,op_add); }
value type_double_sub(value f) { return op2(f,op_sub); }
value type_double_mul(value f) { return op2(f,op_mul); }
value type_double_div(value f) { return op2(f,op_div); }

/* (double_cmp x y) compares x and y and returns -1, 0, or +1. */
value type_double_cmp(value f) { return op2(f,op_cmp); }

/* double_string num */
/*LATER note that sprintf "%.15g" shows 0.0 as "0", which looks like a long if
we try to read it back in.  Perhaps force a ".0" suffix, not sure. */
extern int sprintf(char *str, const char *format, ...);
value type_double_string(value f)
	{
	if (!f->L) return f;

	value x = eval(f->R);

	char buf[100]; /* being careful here */
	/* We show 15 digits because that's what Perl does. */
	sprintf(buf, "%.15g", get_double(x));

	value g = Qstr0(buf);
	drop(x);
	return g;
	}

/*LATER more functions */
#if 0
is_double
double_long
#endif
