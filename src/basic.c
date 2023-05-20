#include <value.h>
#include <lam.h>

#include <basic.h>

static value step_I(value pair)
	{
	return hold(pair->R->L);
	}

struct type type_I = { step_I, no_apply, no_clear };

value I(void)
	{
	return L(new_exp(&type_I));
	}
