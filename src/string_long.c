#include <stdio.h>
#include <stdlib.h>
#include "value.h"
#include "apply.h"
#include "long.h"
#include "string.h"
#include "string_long.h"

/*
Convert string to long.  The value of (string_long string no yes) is:

  no           # if string has incorrect format
  yes long     # if string has correct format

Note:  man strtol says "In particular, if *nptr is not '\0' but **endptr is
'\0' on return, the entire string is valid."

LATER: strtod does not allow numeric separators like ',' and '_'.
*/

int reduce_string_long(struct value *value)
	{
	if (!value->L || !value->L->L || !value->L->L->L) return 0;

	struct atom_string *atom = evaluate_string(value->L->L->R);

	char *nptr = atom->data;
	char *endptr;
	long num = strtol(nptr, &endptr, 10);

	if (*nptr != '\0' && *endptr == '\0')
		replace(value, apply(value->R,new_long(num)));
	else
		replace(value, value->L->R);

	return 1;
	}

struct type type_string_long =
	{ reduce_string_long, hold_both, drop_both, "string_long" };

struct value value_string_long = { 1, &type_string_long, 0, 0 };
