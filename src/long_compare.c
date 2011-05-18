#include "value.h"
#include "compare.h"
#include "long.h"
#include "long_compare.h"

int reduce_long_compare(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	long x = evaluate_long(value->L->R);
	long y = evaluate_long(value->R);

	int cmp = x < y ? -1 : x > y ? 1 : 0;
	replace_compare(value, cmp);
	return 1;
	}

struct type type_long_compare =
	{ reduce_long_compare, hold_both, drop_both, "long_compare" };

struct value value_long_compare = { 1, &type_long_compare, 0, 0 };
