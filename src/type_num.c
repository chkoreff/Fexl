#include <num.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <convert.h>
#include <type_num.h>
#include <type_str.h>

value type_num(value f)
	{
	if (f->N == 0) num_free((number)f->R);
	if (!f->L) return 0;
	return Q(type_void);
	}

value Qnum(number x)
	{
	return D(type_num,x);
	}

value Qnum_str0(const char *name)
	{
	number n = str0_num(name);
	if (n) return Qnum(n);
	return 0;
	}

value type_num_str(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_num)
		f = Qstr(num_str((number)x->R));
	else
		f = Q(type_void);
	drop(x);
	return f;
	}
	}

value type_is_num(value f)
	{
	return op_is_type(f,type_num);
	}
