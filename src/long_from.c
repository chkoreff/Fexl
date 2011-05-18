#include <stdio.h>
#include <stdlib.h>
#include "value.h"
#include "double.h"
#include "long.h"
#include "long_from.h"
#include "string.h"

/*
LATER: strtod does not allow numeric separators like ',' and '_'.
*/

int reduce_long_from(struct value *value)
	{
	if (!value->L) return 0;

	struct value *arg = value->R;
	evaluate(arg);

	if (!arg->L)
		{
		if (arg->T == &type_long)
			{
			replace(value, arg);
			return 0;
			}

		if (arg->T == &type_double)
			{
			struct atom_double *atom = (struct atom_double *)arg->R;
			replace(value, new_long(atom->data));
			return 0;
			}

		if (arg->T == &type_string)
			{
			struct atom_string *atom = (struct atom_string *)arg->R;

			char *end;
			long num = strtol(atom->data, &end, 10);
			if (end - atom->data != atom->len)
				{
				fprintf(stderr, "Invalid long \"%s\"\n", atom->data);
				exit(1);
				}

			replace(value, new_long(num));
			return 0;
			}
		}

	exit_bad_type(arg);
	return 0;
	}

struct type type_long_from =
	{ reduce_long_from, hold_both, drop_both, "long_from" };

struct value value_long_from = { 1, &type_long_from, 0, 0 };
