#include <str.h>
#include <value.h>

#include <basic.h>
#include <format.h>
#include <stdlib.h> // strtod
#include <type_str.h>

#include <type_num.h>

static void clear_num(value f)
	{
	(void)f;
	}

value type_num(value fun, value arg)
	{
	return type_void(fun,arg);
	}

value Qnum(double x)
	{
	static struct value clear = {{.N=0}, {.clear=clear_num}};
	return Qdouble(type_num,&clear,x);
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

unsigned long get_ulong(value x)
	{
	double n = x->v_double;
	return n >= 0 ? n : 0;
	}

value type_num_str(value fun, value arg)
	{
	arg = eval(arg);
	if (arg->T == type_num)
		{
		double x = arg->v_double;
		drop(fun);
		drop(arg);
		return Qstr(str_new_data0(format_double(x)));
		}
	else
		return type_void(fun,arg);
	}
