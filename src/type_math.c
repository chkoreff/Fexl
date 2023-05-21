#include <value.h>
#include <basic.h>
#include <type_num.h>

#include <type_math.h>

// Arithmetic operators

static value op_num2(value pair, double op(double,double))
	{
	value x = pair->R->R->L->L;
	value y = pair->R->L->L;
	if (x->T == &type_num && y->T == &type_num)
		{
		double xn = x->v_double;
		double yn = y->v_double;
		return V(Qnum(op(xn,yn)));
		}
	else
		return V(hold(Qvoid));
	}

static double add(double x, double y) { return x + y; }
static double sub(double x, double y) { return x - y; }
static double mul(double x, double y) { return x * y; }
static double div(double x, double y) { return x / y; }
static double xor(double x, double y) { return (long)x ^ (long)y; }

static value step_add(value pair) { return op_num2(pair,add); }
struct type type_add = { step_add, no_apply, no_clear };

static value step_sub(value pair) { return op_num2(pair,sub); }
struct type type_sub = { step_sub, no_apply, no_clear };

static value step_mul(value pair) { return op_num2(pair,mul); }
struct type type_mul = { step_mul, no_apply, no_clear };

static value step_div(value pair) { return op_num2(pair,div); }
struct type type_div = { step_div, no_apply, no_clear };

static value step_xor(value pair) { return op_num2(pair,xor); }
struct type type_xor = { step_xor, no_apply, no_clear };
