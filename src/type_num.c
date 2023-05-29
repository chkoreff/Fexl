#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <format.h>
#include <stdlib.h> // strtod
#include <type_str.h>

#include <type_num.h>

struct type type_num = { 0, apply_void, no_clear };

value Qnum(double x)
	{
	return Qdouble(&type_num,x);
	}

value Qnum_str0(const char *name)
	{
	char *end;
	double val;
	val = strtod(name, &end);
	if (*end == '\0')
		return Qnum(val);
	else
		return 0;
	}

static value apply_num_str(value f, value x)
	{
	value g;
	x = eval(x);
	if (x->T == &type_num)
		{
		string (*op)(double) = f->v_ptr;
		g = Qstr(op(x->v_double));
		}
	else
		g = hold(Qvoid);

	drop(f);
	drop(x);
	return g;
	}

static struct type type_num_str = { 0, apply_num_str, no_clear };

static void define_num_str(const char *name, string op(double))
	{
	define(name, Q(&type_num_str,op));
	}

static string num_str(double x)
	{
	return str_new_data0(format_double(x));
	}

void use_num(void)
	{
	define_num_str("num_str", num_str);
	}
