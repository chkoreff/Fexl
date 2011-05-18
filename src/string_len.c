#include <string.h>
#include "value.h"
#include "long.h"
#include "string.h"
#include "string_len.h"

int reduce_string_len(struct value *value)
	{
	if (!value->L) return 0;

	struct atom_string *x = evaluate_string(value->R);
	replace(value, new_long(x->len));
	return 0;
	}

struct type type_string_len =
	{ reduce_string_len, hold_both, drop_both, "string_len" };

struct value value_string_len = { 1, &type_string_len, 0, 0 };
