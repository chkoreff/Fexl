#include <stdint.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <convert.h>
#include <format.h>
#include <memory.h>
#include <type_num.h>
#include <type_str.h>

static void num_free(double *p)
	{
	free_memory(p,sizeof(double));
	}

value type_num(value f)
	{
	return type_atom(f);
	}

value Qnum(double x)
	{
	static struct value atom = {0, (type)num_free};
	double *p = new_memory(sizeof(double));
	*p = x;
	return V(type_num,&atom,(value)p);
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
		f = hold(&Qvoid);
	drop(x);
	return f;
	}
	}

value type_is_num(value f)
	{
	return op_is_type(f,type_num);
	}
