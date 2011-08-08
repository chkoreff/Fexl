#include "value.h"
#include "long.h"
#include "long_char.h"
#include "string.h"

/* Convert long to char (string of length 1). */
int reduce_long_char(struct value *value)
	{
	if (!value->L) return 0;

	char buf[1];
	buf[0] = evaluate_long(value->R);
	replace(value, new_chars(buf,1));
	return 0;
	}

struct type type_long_char =
	{ reduce_long_char, hold_both, drop_both, "long_char" };

struct value value_long_char = { 1, &type_long_char, 0, 0 };
