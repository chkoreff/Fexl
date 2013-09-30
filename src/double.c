#include <stdlib.h> /* strtod */
#include "memory.h"

#include "value.h"
#include "basic.h"
#include "double.h"

value type_double(value f)
	{
	if (!f->N)
		{
		double *p = (double *)f->R;
		free_memory(p, sizeof(double));
		}
	return f;
	}

value Qdouble(double val)
	{
	double *p = new_memory(sizeof(double));
	*p = val;
	return V(type_double,0,(value)p);
	}

double get_double(value f)
	{
	if (f->T != type_double) bad_type();
	double *p = (double *)f->R;
	return *p;
	}

static double op_add(double x, double y) { return x + y; }
static double op_sub(double x, double y) { return x - y; }
static double op_mul(double x, double y) { return x * y; }
/*
Note that dividing by zero is no problem here. If you divide a non-zero by
zero, it yields inf (infinity). If you divide zero by zero, it yields -nan
(not a number).
*/
static double op_div(double x, double y) { return x / y; }

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

/* Convert string to double and return true if successful. */
int string_double(const char *beg, double *num)
	{
	char *end;
	*num = strtod(beg, &end);
	return *beg != '\0' && *end == '\0';
	}

/*TODO more functions */
