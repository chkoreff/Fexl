#include <string.h>
#include "value.h"
#include "long.h"
#include "string.h"
#include "string_slice.h"

int reduce_string_slice(struct value *value)
	{
	if (!value->L || !value->L->L || !value->L->L->L) return 0;

	struct atom_string *x = evaluate_string(value->L->L->R);
	long pos = evaluate_long(value->L->R);
	long len = evaluate_long(value->R);

	if (pos < 0)
		{
		len += pos;
		pos = 0;
		}

	if (len > x->len - pos)
		len = x->len - pos;

	replace(value, new_chars(x->data + pos, len));
	return 0;
	}

struct type type_string_slice =
	{ reduce_string_slice, hold_both, drop_both, "string_slice" };

struct value value_string_slice = { 1, &type_string_slice, 0, 0 };
