#include <string.h>
#include "value.h"
#include "long.h"
#include "string.h"
#include "string_at.h"

int reduce_string_at(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	struct atom_string *x = evaluate_string(value->L->R);
	long pos = evaluate_long(value->R);

	long ch = pos >= 0 && pos < x->len ? x->data[pos] : 0;
	replace(value, new_long(ch));
	return 0;
	}

struct type type_string_at =
	{ reduce_string_at, hold_both, drop_both, "string_at" };

struct value value_string_at = { 1, &type_string_at, 0, 0 };
