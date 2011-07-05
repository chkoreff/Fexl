#include "value.h"
#include "apply.h"
#include "Q.h"

/*
The Q function is used for eager evaluation.  It follows this rule:
  (Q x y) = (y x)

However, in this case x is evaluated before y is applied to it.  You can also
think of Q as an "eval" function, meaning "evaluate x and then pass the result
into y."

In Fexl I use "?" for this combinator, but I can't do that in ANSI-C so I use Q
instead.
*/

int reduce_Q(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	evaluate(value->L->R);
	replace(value, apply(value->R,value->L->R));
	return 1;
	}

struct type type_Q = { reduce_Q, hold_both, drop_both, "Q" };
struct value value_Q = { 1, &type_Q, 0, 0 };
