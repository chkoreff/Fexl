#include <stdio.h>
#include <stdlib.h>
#include "value.h"
#include "double.h"
#include "double_from.h"
#include "long.h"
#include "string.h"

/*
LATER: strtod does not allow numeric separators like ',' and '_'.
*/

int reduce_double_from(struct value *value)
	{
	if (!value->L) return 0;

	struct value *arg = value->R;
	evaluate(arg);

	if (!arg->L)
		{
		if (arg->T == &type_double)
			{
			replace(value, arg);
			return 0;
			}

		if (arg->T == &type_long)
			{
			struct atom_long *atom = (struct atom_long *)arg->R;
			replace(value, new_double(atom->data));
			return 0;
			}

		if (arg->T == &type_string)
			{
			struct atom_string *atom = (struct atom_string *)arg->R;

			char *end;
			double num = strtod(atom->data, &end);
			if (end - atom->data != atom->len)
				{
				fprintf(stderr, "Invalid double \"%s\"\n", atom->data);
				exit(1);
				}

			replace(value, new_double(num));
			return 0;
			}
		}

	exit_bad_type(arg);
	return 0;
	}

struct type type_double_from =
	{ reduce_double_from, hold_both, drop_both, "double_from" };

struct value value_double_from = { 1, &type_double_from, 0, 0 };
