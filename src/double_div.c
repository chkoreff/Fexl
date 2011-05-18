#include "value.h"
#include "double.h"
#include "double_div.h"

/*
Note that dividing by zero is no problem here.  If you divide a non-zero by
zero, it yields inf (infinity).  If you divide zero by zero, it yields -nan
(not a number).
*/
int reduce_double_div(struct value *value)
	{
	if (!value->L || !value->L->L) return 0;

	replace(value, new_double(
		evaluate_double(value->L->R) /
		evaluate_double(value->R)));
	return 0;
	}

struct type type_double_div =
	{ reduce_double_div, hold_both, drop_both, "double_div" };

struct value value_double_div = { 1, &type_double_div, 0, 0 };
