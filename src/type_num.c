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
	return type_bad(f);
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

value type_num_str(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	value g;
	if (is_atom(type_num,x))
		g = Qstr(num_str((number)x->R));
	else
		g = hold(bad);
	drop(x);
	return g;
	}
	}

value type_is_num(value f)
	{
	return Qis_type(type_num,f);
	}
