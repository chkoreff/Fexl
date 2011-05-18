#include "value.h"
#include "I.h"

/*
The I function follows the rule (I x) = x.  This is known as the "identity
function."
*/

int reduce_I(struct value *value)
	{
	if (!value->L) return 0;

	replace(value, value->R);
	return 1;
	}

struct type type_I = { reduce_I, hold_both, drop_both, "I" };
struct value value_I = { 1, &type_I, 0, 0 };
