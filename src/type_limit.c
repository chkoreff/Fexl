#include <value.h>

#include <basic.h>
#include <context.h>
#include <limit.h>
#include <type_num.h>

#include <type_limit.h>

static value op_limit(value fun, value arg, void op(unsigned long))
	{
	arg = eval(arg);
	if (arg->T == type_num)
		{
		op(get_ulong(arg));
		return type_F(fun,arg);
		}
	else
		return type_void(fun,arg);
	}

value type_limit_time(value fun, value arg)
	{
	return op_limit(fun,arg,limit_time);
	}

value type_limit_stack(value fun, value arg)
	{
	return op_limit(fun,arg,limit_stack);
	}

value type_limit_memory(value fun, value arg)
	{
	return op_limit(fun,arg,limit_memory);
	}

void use_limit(void)
	{
	define("limit_time", Q(type_limit_time));
	define("limit_stack", Q(type_limit_stack));
	define("limit_memory", Q(type_limit_memory));
	}
