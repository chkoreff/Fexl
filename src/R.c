#include "value.h"
#include "apply.h"
#include "R.h"

/*
The R function follows the rule (R x y z) = (x (y z)).  I call it R because it
passes z to the right side only.  It can be defined as
	S (C S) C
but it's such a common pattern that I make a special combinator for it.
*/

int reduce_R(struct value *value)
	{
	if (!value->L || !value->L->L || !value->L->L->L) return 0;

	replace(value, apply(
		value->L->L->R,
		apply(value->L->R,value->R)));
	return 1;
	}

struct type type_R = { reduce_R, hold_both, drop_both, "R" };
struct value value_R = { 1, &type_R, 0, 0 };
