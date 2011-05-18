#include <string.h>
#include "value.h"
#include "compare.h"
#include "string.h"
#include "string_compare.h"

/*
Note that we can't just use strcmp here because we're comparing binary data.
So we use memcmp to compare the positions in common, and if that compares equal
then we compare the lengths.
*/

int reduce_string_compare(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	struct atom_string *x = evaluate_string(value->L->R);
	struct atom_string *y = evaluate_string(value->R);

	int min_len = x->len < y->len ? x->len : y->len;

	int cmp = memcmp(x->data, y->data, min_len);

	if (cmp == 0)
		cmp = x->len < y->len ? -1 : x->len > y->len ? 1 : 0;

	replace_compare(value, cmp);
	return 1;
	}

struct type type_string_compare =
	{ reduce_string_compare, hold_both, drop_both, "string_compare" };

struct value value_string_compare = { 1, &type_string_compare, 0, 0 };
