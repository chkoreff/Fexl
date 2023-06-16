#include <value.h>

#include <basic.h>
#include <context.h>
#include <math.h> // pow fabs sqrt exp log sin cos M_PI
#include <type_num.h>

#include <type_math.h>

// Unary operators

static value op1(value fun, value arg, double op(double))
	{
	arg = eval(arg);
	if (arg->T == type_num)
		{
		value g = Qnum(op(arg->v_double));
		drop(fun);
		drop(arg);
		return g;
		}
	else
		return type_void(fun,arg);
	}

value type_round(value fun, value arg) { return op1(fun,arg,round); }
value type_ceil(value fun, value arg) { return op1(fun,arg,ceil); }
value type_trunc(value fun, value arg) { return op1(fun,arg,trunc); }
value type_abs(value fun, value arg) { return op1(fun,arg,fabs); }
value type_sqrt(value fun, value arg) { return op1(fun,arg,sqrt); }
value type_exp(value fun, value arg) { return op1(fun,arg,exp); }
value type_log(value fun, value arg) { return op1(fun,arg,log); }
value type_sin(value fun, value arg) { return op1(fun,arg,sin); }
value type_cos(value fun, value arg) { return op1(fun,arg,cos); }

// Binary operators

static value op2(value fun, value arg, double op(double,double))
	{
	if (fun->L == 0)
		return need(fun,arg,type_num);
	else
		{
		arg = eval(arg);
		if (arg->T == type_num)
			{
			value g = Qnum(op(fun->R->v_double, arg->v_double));
			drop(fun);
			drop(arg);
			return g;
			}
		else
			return type_void(fun,arg);
		}
	}

static double add(double x, double y) { return x + y; }
static double sub(double x, double y) { return x - y; }
static double mul(double x, double y) { return x * y; }
static double div(double x, double y) { return x / y; }
static double xor(double x, double y) { return (long)x ^ (long)y; }

value type_add(value fun, value arg) { return op2(fun,arg,add); }
value type_sub(value fun, value arg) { return op2(fun,arg,sub); }
value type_mul(value fun, value arg) { return op2(fun,arg,mul); }
value type_div(value fun, value arg) { return op2(fun,arg,div); }
value type_pow(value fun, value arg) { return op2(fun,arg,pow); }
value type_xor(value fun, value arg) { return op2(fun,arg,xor); }

void use_math(void)
	{
	define("pi", Qnum(M_PI));

	define("round", Q(type_round));
	define("ceil", Q(type_ceil));
	define("trunc", Q(type_trunc));
	define("abs", Q(type_abs));
	define("sqrt", Q(type_sqrt));
	define("exp", Q(type_exp));
	define("log", Q(type_log));
	define("sin", Q(type_sin));
	define("cos", Q(type_cos));

	define("+", Q(type_add));
	define("-", Q(type_sub));
	define("*", Q(type_mul));
	define("/", Q(type_div));
	define("^", Q(type_pow));
	define("xor", Q(type_xor));
	}
