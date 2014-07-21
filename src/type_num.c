#include <value.h>
#include <num.h>
#include <type_num.h>

value type_num(value f)
	{
	if (f->N == 0) num_free((number)f->R);
	return 0;
	}

value Qnum(number x)
	{
	if (!x) return 0;
	{
	value f = D(type_num,x);
	if (!f) num_free(x);
	return f;
	}
	}

value Qnum_ulong(unsigned long val)
	{
	return Qnum(num_new_ulong(val));
	}
