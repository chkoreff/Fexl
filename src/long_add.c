#include "value.h"
#include "long.h"
#include "long_add.h"

int reduce_long_add(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	replace(value, new_long(
		evaluate_long(value->L->R) +
		evaluate_long(value->R)));
	return 0;
	}

struct type type_long_add =
	{ reduce_long_add, hold_both, drop_both, "long_add" };

struct value value_long_add = { 1, &type_long_add, 0, 0 };
