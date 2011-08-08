#include <stdio.h>
#include "value.h"
#include "double.h"
#include "string.h"

/* Convert double to string. */
/* LATER I'd like to use snprintf here but it's not available on my system. */
int reduce_double_string(struct value *value)
	{
	if (!value->L) return 0;

	double data = evaluate_double(value->R);

	char buf[100]; /* being careful here */
	sprintf(buf, "%.15g", data);

	replace(value, new_string(buf));
	return 0;
	}

struct type type_double_string =
	{ reduce_double_string, hold_both, drop_both, "double_string" };

struct value value_double_string = { 1, &type_double_string, 0, 0 };
