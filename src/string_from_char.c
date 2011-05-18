#include "value.h"
#include "long.h"
#include "string.h"
#include "string_from_char.h"

int reduce_string_from_char(struct value *value)
	{
	if (!value->L) return 0;

	char buf[1];
	buf[0] = evaluate_long(value->R);
	replace(value, new_chars(buf,1));
	return 0;
	}

struct type type_string_from_char =
	{ reduce_string_from_char, hold_both, drop_both, "string_from_char" };

struct value value_string_from_char = { 1, &type_string_from_char, 0, 0 };
