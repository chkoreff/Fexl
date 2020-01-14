#include <stdint.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <convert.h>
#include <format.h>
#include <memory.h>
#include <standard.h>
#include <type_num.h>
#include <type_str.h>

value type_num(value f)
	{
	if (f->N == 0)
		{
		free_memory(f->R,sizeof(double));
		return 0;
		}
	return type_void(f);
	}

value Qnum(double x)
	{
	double *p = new_memory(sizeof(double));
	*p = x;
	return D(type_num,p);
	}

value Qnum_str0(const char *name)
	{
	double x;
	if (str0_double(name,&x))
		return Qnum(x);
	else
		return 0;
	}

double get_double(value x)
	{
	return *((double *)x->R);
	}

unsigned long get_ulong(value x)
	{
	double n = get_double(x);
	return n >= 0 ? n : 0;
	}

value type_num_str(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = Qstr(str_new_data0(format_double(get_double(x))));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value type_is_num(value f)
	{
	return op_is_type(f,type_num);
	}
