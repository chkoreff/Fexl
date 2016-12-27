#include <num.h>
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
		op(get_double(x));
		f = hold(&QI);
		}
	else
		f = reduce_void(f);
	drop(x);
	return f;
	}
	}

value type_limit_time(value f) { return op_limit(f,limit_time); }
value type_limit_stack(value f) { return op_limit(f,limit_stack); }
value type_limit_memory(value f) { return op_limit(f,limit_memory); }

/* LATER RLIMIT_ FSIZE LOCKS NOFILE NPROC */
