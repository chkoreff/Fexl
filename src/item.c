#include "value.h"
#include "apply.h"
#include "item.h"

/*
(item head tail) is the list consisting of the head element followed by the
tail list.  It can be defined as:
	\item = (\head\tail \end\item item head tail)

In combinatorial form that is:
	\item = (R (R C) pair)

It's such a common pattern that I make a special combinator for it.  It seems
to accelerate some functions by about 15% or so.
*/

int reduce_item(struct value *value)
	{
	if (!value->L || !value->L->L || !value->L->L->L || !value->L->L->L->L)
		return 0;

	replace(value, apply(apply(value->R,value->L->L->L->R),value->L->L->R));
	return 1;
	}

struct type type_item = { reduce_item, hold_both, drop_both, "item" };
struct value value_item = { 1, &type_item, 0, 0 };
