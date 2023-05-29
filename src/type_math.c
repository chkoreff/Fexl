#include <value.h>

#include <basic.h>
#include <context.h>
#include <math.h> // pow fabs sqrt exp log sin cos M_PI
#include <type_num.h>

#include <type_math.h>

// Unary operators

static value apply_num_num(value f, value x)
	{
	value g;
	x = eval(x);
	if (x->T == &type_num)
		{
		double (*op)(double) = f->v_ptr;
		g = Qnum(op(x->v_double));
		}
	else
		g = hold(Qvoid);

	drop(f);
	drop(x);
	return g;
	}

static struct type type_num_num = { 0, apply_num_num, no_clear };

static void define_num_num(const char *name, double op(double))
	{
	define(name, Q(&type_num_num,op));
	}

// Binary operators

static value apply_num_num_num(value f, value x)
	{
	value g;

	x = eval(x);
	if (f->L == 0)
		{
		if (x->T == &type_num)
			g = V(f->T,hold(f),hold(x));
		else
			g = hold(Qvoid);
		}
	else
		{
		if (x->T == &type_num)
			{
			double (*op)(double,double) = f->L->v_ptr;
			g = Qnum(op(f->R->v_double, x->v_double));
			}
		else
			g = hold(Qvoid);
		}

	drop(f);
	drop(x);
	return g;
	}

static struct type type_num_num_num = { 0, apply_num_num_num, clear_T };

static void def2(const char *name, double op(double,double))
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

	def2("+", add);
	def2("-", sub);
	def2("*", mul);
	def2("/", div);
	def2("^", pow);
	def2("xor", xor);
	}
