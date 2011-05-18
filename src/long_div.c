#include <stdio.h>
#include <stdlib.h>
#include "value.h"
#include "long.h"
#include "long_div.h"

int reduce_long_div(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	long x = evaluate_long(value->L->R);
	long y = evaluate_long(value->R);

	if (y == 0)
		{
		fprintf(stderr, "Attempt to divide by zero.\n");
		exit(1);
		}

	replace(value, new_long(x / y));
	return 0;
	}

struct type type_long_div =
	{ reduce_long_div, hold_both, drop_both, "long_div" };

struct value value_long_div = { 1, &type_long_div, 0, 0 };
