#include <math.h> // pow fabs sqrt exp log sin cos M_PI
#include <value.h>

#include <basic.h>
#include <type_math.h>
#include <type_num.h>

// Explicitly declare certain functions which are missing from math.h on some
// machines.
extern double round(double);
extern double trunc(double);

static value op_num(value f, double op(double))
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = Qnum(op(x->v_double));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value op_num_num(value f, double op(double,double))
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
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

static value type_add(value f) { return op_num_num(f,add); }
static value type_sub(value f) { return op_num_num(f,sub); }
static value type_mul(value f) { return op_num_num(f,mul); }
static value type_div(value f) { return op_num_num(f,div); }
static value type_pow(value f) { return op_num_num(f,pow); }
static value type_xor(value f) { return op_num_num(f,xor); }
static value type_round(value f) { return op_num(f,round); }
static value type_ceil(value f) { return op_num(f,ceil); }
static value type_trunc(value f) { return op_num(f,trunc); }
static value type_abs(value f) { return op_num(f,fabs); }
static value type_sqrt(value f) { return op_num(f,sqrt); }
static value type_exp(value f) { return op_num(f,exp); }
static value type_log(value f) { return op_num(f,log); }
static value type_sin(value f) { return op_num(f,sin); }
static value type_cos(value f) { return op_num(f,cos); }
static value type_tan(value f) { return op_num(f,tan); }
static value type_atan(value f) { return op_num(f,atan); }

static const double num_pi = M_PI;

void define_math(void)
	{
	define("+",Q(type_add));
	define("-",Q(type_sub));
	define("*",Q(type_mul));
	define("/",Q(type_div));
	define("^",Q(type_pow));
	define("xor",Q(type_xor));
	define("round",Q(type_round));
	define("ceil",Q(type_ceil));
	define("trunc",Q(type_trunc));
	define("abs",Q(type_abs));
	define("sqrt",Q(type_sqrt));
	define("exp",Q(type_exp));
	define("log",Q(type_log));
	define("sin",Q(type_sin));
	define("cos",Q(type_cos));
	define("tan",Q(type_tan));
	define("atan",Q(type_atan));
	define("pi",Qnum(num_pi));
	}
