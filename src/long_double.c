#include "value.h"
#include "double.h"
#include "long.h"
#include "long_double.h"

/* Convert long to double. */
int reduce_long_double(struct value *value)
	{
	if (!value->L) return 0;

	long data = evaluate_long(value->R);
	replace(value, new_double(data));
	return 0;
	}

struct type type_long_double =
	{ reduce_long_double, hold_both, drop_both, "long_double" };

struct value value_long_double = { 1, &type_long_double, 0, 0 };
