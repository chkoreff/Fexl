#include "value.h"
#include "apply.h"
#include "compare.h"
#include "C.h"
#include "I.h"
#include "R.h"

/*
I tried making built-in combinators for the forms (R C C), (C C), and (C (C I))
used here, but it had a negligible effect on performance, and in some cases
even seemed slightly slower.  Maybe the built-in combinators had poorer
locality of reference, which wiped out any gains we got from fewer calls to
apply and fewer reduction steps.
*/

void replace_compare(struct value *value, int cmp)
	{
	if (cmp < 0)
		{
		replace(value, apply(apply(&value_R,&value_C),&value_C));
		return;
		}

	if (cmp > 0)
		{
		replace(value, apply(&value_C,apply(&value_C,&value_I)));
		return;
		}

	replace(value, apply(&value_C,&value_C));
	}
