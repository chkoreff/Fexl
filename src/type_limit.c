#include <value.h>

#include <basic.h>
#include <limit.h>
#include <type_limit.h>
#include <type_num.h>

static value op_limit(value f, void op(unsigned long))
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
	}

static value type_limit_time(value f)
	{
	return op_limit(f,limit_time);
	}

static value type_limit_stack(value f)
	{
	return op_limit(f,limit_stack);
	}

static value type_limit_memory(value f)
	{
	return op_limit(f,limit_memory);
	}

void define_limit(void)
	{
	define("limit_time",Q(type_limit_time));
	define("limit_stack",Q(type_limit_stack));
	define("limit_memory",Q(type_limit_memory));
	}
