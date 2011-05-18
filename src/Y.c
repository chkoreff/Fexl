#include "value.h"
#include "apply.h"
#include "Y.h"

/*
The Y function follows the rule (Y f) = (f (Y f)).  This is known as the
"fixpoint function."
*/

int reduce_Y(struct value *value)
	{
	if (!value->L) return 0;

	replace(value, apply(value->R,apply(value->L,value->R)));
	return 1;
	}

struct type type_Y = { reduce_Y, hold_both, drop_both, "Y" };
struct value value_Y = { 1, &type_Y, 0, 0 };
