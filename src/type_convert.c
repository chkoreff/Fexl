#include <value.h>
#include <basic.h>
#include <format.h>
#include <type_num.h>
#include <str.h>
#include <type_str.h>

#include <type_convert.h>

static value step_num_str(value pair)
	{
	value x = pair->R->L->L;
	if (x->T == &type_num)
		return V(Qstr0(format_double(x->v_double)));
	else
		return V(hold(Qvoid));
	}

struct type type_num_str = { step_num_str, no_apply, no_clear };
