#include "value.h"
#include "apply.h"
#include "L.h"

/*
The L function follows the rule (L x y z) = (x z y).  I call it L because it
passes z to the left side only.  It can be defined as
	S (S (C S) (S (C C) S)) (C C)
but it's such a common pattern that I make a special combinator for it.
*/

int reduce_L(struct value *value)
	{
	if (!value->L || !value->L->L || !value->L->L->L) return 0;

	replace(value, apply(
		apply(value->L->L->R,value->R),
		value->L->R));
	return 1;
	}

struct type type_L = { reduce_L, hold_both, drop_both, "L" };
struct value value_L = { 1, &type_L, 0, 0 };
