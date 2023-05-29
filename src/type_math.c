#include <value.h>

#include <basic.h>
#include <context.h>
#include <math.h> // pow fabs sqrt exp log sin cos M_PI
#include <type_num.h>

#include <type_math.h>

// Unary operators

static value apply_num_num(value f, value x)
	{
	x = eval(x);
	if (x->T == &type_num)
		{
		double (*op)(double) = f->v_ptr;
		return next(f,x, Qnum(op(x->v_double)));
		}
	else
		return apply_void(f,x);
	}

static struct type type_num_num = { 0, apply_num_num, no_clear };

static void define_num_num(const char *name, double op(double))
	{
	define(name, Q(&type_num_num,op));
	}

// Binary operators

static value apply_num_num_num(value f, value x)
	{
	if (f->L == 0)
		return need(f,x,&type_num);
	else
		{
		x = eval(x);
		if (x->T == &type_num)
			{
			double (*op)(double,double) = f->L->v_ptr;
			return next(f,x, Qnum(op(f->R->v_double, x->v_double)));
			}
		else
			return apply_void(f,x);
		}
	}

static struct type type_num_num_num = { 0, apply_num_num_num, clear_T };

static void define_num_num_num(const char *name, double op(double,double))
	{
	define(name, Q(&type_num_num_num,op));
	}

static double add(double x, double y) { return x + y; }
static double sub(double x, double y) { return x - y; }
static double mul(double x, double y) { return x * y; }
static double div(double x, double y) { return x / y; }
static double xor(double x, double y) { return (long)x ^ (long)y; }

void use_math(void)
	{
	define("pi", Qnum(M_PI));

	define_num_num("round", round);
	define_num_num("ceil", ceil);
	define_num_num("trunc", trunc);
	define_num_num("abs", fabs);
	define_num_num("sqrt", sqrt);
	define_num_num("exp", exp);
	define_num_num("log", log);
	define_num_num("sin", sin);
	define_num_num("cos", cos);

	define_num_num_num("+", add);
	define_num_num_num("-", sub);
	define_num_num_num("*", mul);
	define_num_num_num("/", div);
	define_num_num_num("^", pow);
	define_num_num_num("xor", xor);
	}
