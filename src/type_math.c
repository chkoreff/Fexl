#include <value.h>

#include <basic.h>
#include <context.h>
#include <math.h> // pow fabs sqrt exp log sin cos M_PI
#include <type_num.h>

#include <type_math.h>

// Unary operators

static value apply_num(value f, value x)
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

static struct type op_num = { 0, apply_num, no_clear };

static void def1(const char *name, double op(double))
	{
	define(name, Q(&op_num,op));
	}

// Binary operators

static value apply_num_num(value f, value x)
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

static struct type op_num_num = { 0, apply_num_num, clear_T };

static void def2(const char *name, double op(double,double))
	{
	define(name, Q(&op_num_num,op));
	}

static double add(double x, double y) { return x + y; }
static double sub(double x, double y) { return x - y; }
static double mul(double x, double y) { return x * y; }
static double div(double x, double y) { return x / y; }
static double xor(double x, double y) { return (long)x ^ (long)y; }

void def_type_math(void)
	{
	define("pi", Qnum(M_PI));

	def1("round", round);
	def1("ceil", ceil);
	def1("trunc", trunc);
	def1("abs", fabs);
	def1("sqrt", sqrt);
	def1("exp", exp);
	def1("log", log);
	def1("sin", sin);
	def1("cos", cos);

	def2("+", add);
	def2("-", sub);
	def2("*", mul);
	def2("/", div);
	def2("^", pow);
	def2("xor", xor);
	}
