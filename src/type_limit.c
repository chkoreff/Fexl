#include <value.h>

#include <basic.h>
#include <context.h>
#include <limit.h>
#include <type_num.h>

#include <type_limit.h>

static value apply_limit(value f, value x)
	{
	x = eval(x);
	if (x->T == &type_num)
		{
		void (*op)(unsigned long) = f->v_ptr;
		op(get_ulong(x));
		drop(x);
		return hold(QI);
		}
	else
		{
		drop(x);
		return hold(Qvoid);
		}
	}

static struct type type_limit = { 0, apply_limit, no_clear };

void use_limit(void)
	{
	define("limit_time", Q(&type_limit,limit_time));
	define("limit_stack", Q(&type_limit,limit_stack));
	define("limit_memory", Q(&type_limit,limit_memory));
	}
