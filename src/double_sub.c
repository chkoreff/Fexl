#include "value.h"
#include "double.h"
#include "double_sub.h"

int reduce_double_sub(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	replace(value, new_double(
		evaluate_double(value->L->R) -
		evaluate_double(value->R)));
	return 0;
	}

struct type type_double_sub =
	{ reduce_double_sub, hold_both, drop_both, "double_sub" };

struct value value_double_sub = { 1, &type_double_sub, 0, 0 };
