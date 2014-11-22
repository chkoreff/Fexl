#include <num.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <convert.h>
#include <type_num.h>
#include <type_str.h>

value type_num(value f)
	{
	if (!f->L) return 0;
	replace_void(f);
	return 0;
	}

value Qnum(number x)
	{
	return D(type_num,x,(type)num_free);
	}

value Qnum_str0(const char *name)
	{
	number n = str0_num(name);
	if (n) return Qnum(n);
	return 0;
	}

void replace_num(value f, number x)
	{
	replace_D(f,type_num,x,(type)num_free);
	}

value type_num_str(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_num)
		replace_str(f, num_str((number)x->R->R));
	else
		replace_void(f);
	drop(x);
	return 0;
	}
	}

value type_is_num(value f)
	{
	return op_is_type(f,type_num);
	}
