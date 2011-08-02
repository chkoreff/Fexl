#include "value.h"
#include "apply.h"
#include "pair.h"

/*
(pair x y) is the ordered pair of x and y.  It can be defined as:
	\pair = (\x\y\p p x y)

In combinatorial form that is:
	\pair = (R L (L I))

It's such a common pattern that I make a special combinator for it.  It seems
to accelerate some functions by about 10% or so.
*/

int reduce_pair(struct value *value)
	{
	if (!value->L || !value->L->L || !value->L->L->L) return 0;

	replace(value, apply(apply(value->R,value->L->L->R),value->L->R));
	return 1;
	}

struct type type_pair = { reduce_pair, hold_both, drop_both, "pair" };
struct value value_pair = { 1, &type_pair, 0, 0 };
