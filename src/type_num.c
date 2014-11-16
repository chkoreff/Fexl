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
	if (!f->L) return f;
	return Q(type_void);
	}

value Qnum(number x)
	{
	return D(type_num,x);
	}

value Qnum_ulong(unsigned long val)
	{
	return Qnum(num_new_ulong(val));
	}

value Qnum_str0(const char *name)
	{
	number n = str0_num(name);
	if (n) return Qnum(n);
	return 0;
	}

value type_num_str(value f)
	{
	if (!f->L) return f;
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
	return is_type(f,type_num);
	}
