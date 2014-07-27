#include <value.h>
#include <basic.h>
#include <num.h>
#include <str.h>
#include <type_num.h>
#include <type_str.h>

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

static string num_str(number x)
	{
	return str_new_data0(format_num(x));
	}

value type_num_str(value f)
	{
	if (!f->L) return 0;
	{
	number x = atom(type_num,arg(&f->R));
	if (!x) return 0;
	return Qstr(num_str(x));
	}
	}

value type_is_num(value f)
	{
	if (!f->L) return 0;
	{
	number x = atom(type_num,arg(&f->R));
	return x ? C : F;
	}
	}
