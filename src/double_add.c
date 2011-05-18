#include "value.h"
#include "double.h"
#include "double_add.h"

int reduce_double_add(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	replace(value, new_double(
		evaluate_double(value->L->R) +
		evaluate_double(value->R)));
	return 0;
	}

struct type type_double_add =
	{ reduce_double_add, hold_both, drop_both, "double_add" };

struct value value_double_add = { 1, &type_double_add, 0, 0 };
