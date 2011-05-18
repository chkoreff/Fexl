#include "value.h"
#include "long.h"
#include "long_sub.h"

int reduce_long_sub(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	replace(value, new_long(
		evaluate_long(value->L->R) -
		evaluate_long(value->R)));
	return 0;
	}

struct type type_long_sub =
	{ reduce_long_sub, hold_both, drop_both, "long_sub" };

struct value value_long_sub = { 1, &type_long_sub, 0, 0 };
