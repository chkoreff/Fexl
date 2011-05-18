#include "value.h"
#include "compare.h"
#include "double.h"
#include "double_compare.h"

int reduce_double_compare(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	double x = evaluate_double(value->L->R);
	double y = evaluate_double(value->R);

	int cmp = x < y ? -1 : x > y ? 1 : 0;
	replace_compare(value, cmp);
	return 1;
	}

struct type type_double_compare =
	{ reduce_double_compare, hold_both, drop_both, "double_compare" };

struct value value_double_compare = { 1, &type_double_compare, 0, 0 };
