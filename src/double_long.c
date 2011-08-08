#include "value.h"
#include "double.h"
#include "long.h"
#include "double_long.h"

/* Convert double to long. */
int reduce_double_long(struct value *value)
	{
	if (!value->L) return 0;

	double data = evaluate_double(value->R);
	replace(value, new_long(data));
	return 0;
	}

struct type type_double_long =
	{ reduce_double_long, hold_both, drop_both, "double_long" };

struct value value_double_long = { 1, &type_double_long, 0, 0 };
