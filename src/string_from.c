#include <stdio.h>
#include "value.h"
#include "double.h"
#include "long.h"
#include "string.h"
#include "string_from.h"

/* LATER I'd like to use snprintf here but it's not available on my system. */

int reduce_string_from(struct value *value)
	{
	if (!value->L) return 0;

	struct value *arg = value->R;
	evaluate(arg);

	if (!arg->L)
		{
		if (arg->T == &type_string)
			{
			replace(value, arg);
			return 0;
			}

		if (arg->T == &type_long)
			{
			struct atom_long *atom = (struct atom_long *)arg->R;

			char buf[100]; /* being careful here */
			sprintf(buf, "%ld", atom->data);

			replace(value, new_string(buf));
			return 0;
			}

		if (arg->T == &type_double)
			{
			struct atom_double *atom = (struct atom_double *)arg->R;

			char buf[100]; /* being careful here */
			sprintf(buf, "%.15g", atom->data);

			replace(value, new_string(buf));
			return 0;
			}
		}

	exit_bad_type(arg);
	return 0;
	}

struct type type_string_from =
	{ reduce_string_from, hold_both, drop_both, "string_from" };

struct value value_string_from = { 1, &type_string_from, 0, 0 };
