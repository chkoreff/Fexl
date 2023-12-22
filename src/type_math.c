#include <math.h> // pow fabs sqrt exp log sin cos M_PI
#include <value.h>

#include <basic.h>
#include <type_math.h>
#include <type_num.h>

// Explicitly declare certain functions which are missing from math.h on some
// machines.
extern double round(double);
extern double trunc(double);

static value op_num(value fun, value f, double op(double))
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = Qnum(op(x->v_double));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	(void)fun;
	}

static value op_num_num(value fun, value f, double op(double,double))
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value x = arg(fun->R);
	value y = arg(f->R);
	if (x->T == type_num && y->T == type_num)
		f = Qnum(op(x->v_double,y->v_double));
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

value type_add(value fun, value f) { return op_num_num(fun,f,add); }
value type_sub(value fun, value f) { return op_num_num(fun,f,sub); }
value type_mul(value fun, value f) { return op_num_num(fun,f,mul); }
value type_div(value fun, value f) { return op_num_num(fun,f,div); }
value type_pow(value fun, value f) { return op_num_num(fun,f,pow); }
value type_xor(value fun, value f) { return op_num_num(fun,f,xor); }
value type_round(value fun, value f) { return op_num(fun,f,round); }
value type_ceil(value fun, value f) { return op_num(fun,f,ceil); }
value type_trunc(value fun, value f) { return op_num(fun,f,trunc); }
value type_abs(value fun, value f) { return op_num(fun,f,fabs); }
value type_sqrt(value fun, value f) { return op_num(fun,f,sqrt); }
value type_exp(value fun, value f) { return op_num(fun,f,exp); }
value type_log(value fun, value f) { return op_num(fun,f,log); }
value type_sin(value fun, value f) { return op_num(fun,f,sin); }
value type_cos(value fun, value f) { return op_num(fun,f,cos); }

const double num_pi = M_PI;
