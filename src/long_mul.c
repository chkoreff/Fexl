#include "value.h"
#include "long.h"
#include "long_mul.h"

int reduce_long_mul(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	replace(value, new_long(
		evaluate_long(value->L->R) *
		evaluate_long(value->R)));
	return 0;
	}

struct type type_long_mul =
	{ reduce_long_mul, hold_both, drop_both, "long_mul" };

struct value value_long_mul = { 1, &type_long_mul, 0, 0 };
