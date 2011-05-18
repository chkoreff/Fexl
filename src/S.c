#include "value.h"
#include "apply.h"
#include "S.h"

/*
The S function follows the rule (((S x) y) z) = ((x z) (y z)).  This is known
as the "fusion function", or "Verschmelzungfunktion" in the original German.
*/

int reduce_S(struct value *value)
	{
	if (!value->L || !value->L->L || !value->L->L->L) return 0;

	replace(value, apply(
		apply(value->L->L->R,value->R),
		apply(value->L->R,value->R)));
	return 1;
	}

struct type type_S = { reduce_S, hold_both, drop_both, "S" };
struct value value_S = { 1, &type_S, 0, 0 };
