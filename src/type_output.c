#include <value.h>
#include <basic.h>

#include <str.h>
#include <type_str.h>

#include <stdio.h>
#include <output.h>

#include <type_output.h>

static value step_say(value pair)
	{
	value x = pair->R->L->L;
	if (x->T == &type_str)
		{
		put_str(x->v_ptr);
		put_ch('\n');
		}
	return V(hold(QI));
	}

struct type type_say = { step_say, no_apply, no_clear };
