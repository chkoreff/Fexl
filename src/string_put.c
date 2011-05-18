#include <unistd.h>
#include "value.h"
#include "string.h"
#include "string_put.h"

/*
Note that we have to use the raw write here because we may be writing binary
data.
*/

int reduce_string_put(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	struct atom_string *atom = evaluate_string(value->L->R);
	if (write(STDOUT_FILENO, atom->data, atom->len)) ;

	replace(value, value->R);
	return 1;
	}

struct type type_string_put =
	{ reduce_string_put, hold_both, drop_both, "string_put" };
struct value value_string_put = { 1, &type_string_put, 0, 0 };
