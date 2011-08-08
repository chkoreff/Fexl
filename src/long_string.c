#include <stdio.h>
#include "value.h"
#include "long.h"
#include "string.h"

/* Convert long to string. */
/* LATER I'd like to use snprintf here but it's not available on my system. */
int reduce_long_string(struct value *value)
	{
	if (!value->L) return 0;

	long data = evaluate_long(value->R);

	char buf[100]; /* being careful here */
	sprintf(buf, "%ld", data);

	replace(value, new_string(buf));
	return 0;
	}

struct type type_long_string =
	{ reduce_long_string, hold_both, drop_both, "long_string" };

struct value value_long_string = { 1, &type_long_string, 0, 0 };
