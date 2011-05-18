#include <string.h>
#include "value.h"
#include "string.h"
#include "string_append.h"

int reduce_string_append(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	struct atom_string *x = evaluate_string(value->L->R);
	struct atom_string *y = evaluate_string(value->R);

	int len = x->len + y->len;

	struct value *result = new_chars(0, len);
	struct atom_string *z = (struct atom_string *)result->R;

	memcpy(z->data, x->data, x->len);
	memcpy(z->data + x->len, y->data, y->len);
	z->data[len] = '\000'; /* add trailing NUL byte */

	replace(value, result);
	return 0;
	}

struct type type_string_append =
	{ reduce_string_append, hold_both, drop_both, "string_append" };

struct value value_string_append = { 1, &type_string_append, 0, 0 };
