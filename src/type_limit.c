#include <value.h>

#include <basic.h>
#include <limit.h>
#include <type_limit.h>
#include <type_num.h>

static value op_limit(value fun, value f, void op(unsigned long))
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		op(get_ulong(x));
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	(void)fun;
	}

value type_limit_time(value fun, value f)
	{
	return op_limit(fun,f,limit_time);
	}

value type_limit_stack(value fun, value f)
	{
	return op_limit(fun,f,limit_stack);
	}

value type_limit_memory(value fun, value f)
	{
	return op_limit(fun,f,limit_memory);
	}
