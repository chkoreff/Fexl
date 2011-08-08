#include <stdio.h>
#include <stdlib.h>
#include "value.h"
#include "apply.h"
#include "double.h"
#include "string.h"
#include "string_double.h"

/*
Convert string to double.  The value of (string_double string no yes) is:

  no           # if string has incorrect format
  yes double   # if string has correct format

LATER: strtod does not allow numeric separators like ',' and '_'.
*/

int reduce_string_double(struct value *value)
	{
	if (!value->L || !value->L->L || !value->L->L->L) return 0;

	struct atom_string *atom = evaluate_string(value->L->L->R);

	char *nptr = atom->data;
	char *endptr;
	double num = strtod(atom->data, &endptr);

	if (*nptr != '\0' && *endptr == '\0')
		replace(value, apply(value->R,new_double(num)));
	else
		replace(value, value->L->R);

	return 1;
	}

struct type type_string_double =
	{ reduce_string_double, hold_both, drop_both, "string_double" };

struct value value_string_double = { 1, &type_string_double, 0, 0 };
