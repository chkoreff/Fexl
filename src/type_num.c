#include <num.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <convert.h>
#include <type_num.h>
#include <type_str.h>

value type_num(value f)
	{
	if (f->N == 0)
		{
		num_free(get_num(f));
		return 0;
		}
	return type_void(f);
	}

value Qnum(number x)
	{
	return D(type_num,x);
	}

value Qnum0(double x)
	{
	return Qnum(num_new_double(x));
	}

value Qnum_str0(const char *name)
	{
	number n = str0_num(name);
	if (n) return Qnum(n);
	return 0;
	}

number get_num(value x)
	{
	return (number)x->R;
	}

double get_double(value x)
	{
	return *get_num(x);
	}

value reduce_num(value f, number x)
	{
	return reduce_D(f,type_num,x);
	}

value type_num_str(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = reduce_str(f,num_str(get_num(x)));
	else
		f = reduce_void(f);
	drop(x);
	return f;
	}
	}

value type_is_num(value f)
	{
	return op_is_type(f,type_num);
	}
