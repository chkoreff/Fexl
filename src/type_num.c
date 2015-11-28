#include <num.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <convert.h>
#include <type_num.h>
#include <type_str.h>

value type_num(value f)
	{
	return type_void(f);
	}

value Qnum(number x)
	{
	return D(type_num,x,(type)num_free);
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

void reduce_num(value f, number x)
	{
	reduce_D(f,type_num,x,(type)num_free);
	}

value type_num_str(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		reduce_str(f, num_str(data(x)));
	else
		reduce_void(f);
	drop(x);
	return 0;
	}
	}

value type_is_num(value f)
	{
	return op_is_type(f,type_num);
	}
