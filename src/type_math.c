#include <math.h> /* pow fabs sqrt exp log sin cos M_PI */
#include <value.h>

#include <standard.h>
#include <type_math.h>
#include <type_num.h>

/* Explicitly declare certain functions which are missing from math.h on some
machines. */
extern double round(double);
extern double trunc(double);

static value op_num(value f, double op(double))
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = Qnum(op(get_double(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

static value op_num_num(value f, double op(double,double))
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_num && y->T == type_num)
		f = Qnum(op(get_double(x),get_double(y)));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static double add(double x, double y) { return x + y; }
static double sub(double x, double y) { return x - y; }
static double mul(double x, double y) { return x * y; }
static double div(double x, double y) { return x / y; }
static double xor(double x, double y) { return (long)x ^ (long)y; }

value type_add(value f) { return op_num_num(f,add); }
value type_sub(value f) { return op_num_num(f,sub); }
value type_mul(value f) { return op_num_num(f,mul); }
value type_div(value f) { return op_num_num(f,div); }
value type_pow(value f) { return op_num_num(f,pow); }
value type_xor(value f) { return op_num_num(f,xor); }
value type_round(value f) { return op_num(f,round); }
value type_ceil(value f) { return op_num(f,ceil); }
value type_trunc(value f) { return op_num(f,trunc); }
value type_abs(value f) { return op_num(f,fabs); }
value type_sqrt(value f) { return op_num(f,sqrt); }
value type_exp(value f) { return op_num(f,exp); }
value type_log(value f) { return op_num(f,log); }
value type_sin(value f) { return op_num(f,sin); }
value type_cos(value f) { return op_num(f,cos); }

const double num_pi = M_PI;
