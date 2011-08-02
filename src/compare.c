#include "value.h"
#include "apply.h"
#include "compare.h"
#include "C.h"
#include "I.h"
#include "R.h"

/* I've found that it's faster *not* to cache the values (R C C), (C C), and
(C (C I)) here.
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
