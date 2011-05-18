#include "value.h"
#include "apply.h"
#include "compare.h"
#include "C.h"
#include "I.h"
#include "R.h"

/* LATER Perhaps cache the values (R C C), (C C), and (C (C I))?
Problem is, we'd have to free them at the end.  We could have a single
"persist" list which is dropped at the end of the program run.

Or maybe just a single globals.c file which we start and finish normally,
and create all the specific values that are needed anywhere.  I'm not usually
a fan of that kind of centralization, but these are universally defined
functions after all.
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
