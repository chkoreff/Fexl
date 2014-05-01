#include <value.h>

#include <num.h>
#include <type_num.h>

value type_num(value f)
	{
	if (!f->N) num_free((number)f->R);
	return 0;
	}

value Qnum(number p)
	{
	return D(type_num,p);
	}

value Qnum_ulong(unsigned long val)
	{
	return Qnum(num_new_ulong(val));
	}

number get_num(value f)
	{
	return (number)get_data(f,type_num);
	}
