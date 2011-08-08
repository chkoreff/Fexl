#include "value.h"
#include "double.h"
#include "type.h"

/* Return true if the argument is of type double.  */
int reduce_double_type(struct value *value)
	{
	return reduce_type_check(value, &type_double);
	}

struct type type_double_type =
	{ reduce_double_type, hold_both, drop_both, "double_type" };

struct value value_double_type = { 1, &type_double_type, 0, 0 };
