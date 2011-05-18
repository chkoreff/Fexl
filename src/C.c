#include "value.h"
#include "C.h"

/*
The C function follows the rule ((C x) y) = x.  This is known as the "constancy
function", or "Konstanzfunktion" in the original German.
*/

int reduce_C(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	replace(value, value->L->R);
	return 1;
	}

struct type type_C = { reduce_C, hold_both, drop_both, "C" };
struct value value_C = { 1, &type_C, 0, 0 };
