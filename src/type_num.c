#include <stdint.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <convert.h>
#include <format.h>
#include <memory.h>
#include <type_num.h>
#include <type_str.h>

static void clear_num(value f)
	{
	(void)f;
	}

value type_num(value fun, value f)
	{
	return type_data(fun,f);
	}

value Qnum(double x)
	{
	static struct value clear = {{.N=0}, {.clear=clear_num}};
	return Qdouble(type_num,&clear,x);
	}

value Qnum_str0(const char *name)
	{
	double x;
	if (str0_double(name,&x))
		return Qnum(x);
	else
		return 0;
	}

unsigned long get_ulong(value x)
	{
	double n = x->v_double;
	return n >= 0 ? n : 0;
	}

value type_num_str(value fun, value f)
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = Qstr(str_new_data0(format_double(x->v_double)));
	else
		f = hold(Qvoid);
	drop(x);
	(void)fun;
	return f;
	}

value type_is_num(value fun, value f)
	{
	return op_is_type(fun,f,type_num);
	}
