#include <string.h>
#include "value.h"
#include "long.h"
#include "string.h"
#include "string_common.h"

/*
Compute the length of the longest common prefix of two strings.
*/

int reduce_string_common(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	struct atom_string *x = evaluate_string(value->L->R);
	struct atom_string *y = evaluate_string(value->R);

	int min_len = x->len < y->len ? x->len : y->len;

	int len = 0;
	while (len < min_len)
		{
		if (x->data[len] != y->data[len]) break;
		len++;
		}

	replace(value, new_long(len));
	return 0;
	}

struct type type_string_common =
	{ reduce_string_common, hold_both, drop_both, "string_common" };

struct value value_string_common = { 1, &type_string_common, 0, 0 };
