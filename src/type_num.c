#include <value.h>
#include <basic.h>
#include <num.h>
#include <type_num.h>

void type_num(value f)
	{
	replace_void(f);
	}

value Qnum(number p)
	{
	return D(type_num,(type)num_free,(value)p);
	}

value Qnum_ulong(unsigned long val)
	{
	return Qnum(num_new_ulong(val));
	}

number get_num(value f)
	{
	return (number)get_D(f,type_num);
	}

void replace_num(value f, number x)
	{
	replace_D(f,type_num,(type)num_free,(value)x);
	}
