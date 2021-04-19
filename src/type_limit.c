#include <value.h>

#include <basic.h>
#include <limit.h>
#include <type_limit.h>
#include <type_num.h>

static value op_limit(value f, void op(unsigned long))
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		op(get_ulong(x));
		f = hold(&QI);
		}
	else
		f = hold(&Qvoid);
	drop(x);
	return f;
	}
	}

value type_limit_time(value f) { return op_limit(f,limit_time); }
value type_limit_stack(value f) { return op_limit(f,limit_stack); }
value type_limit_memory(value f) { return op_limit(f,limit_memory); }
