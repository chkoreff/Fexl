#include "value.h"
#include "long.h"
#include "type.h"

/* Return true if the argument is of type long.  */
int reduce_long_type(struct value *value)
	{
	return reduce_type_check(value, &type_long);
	}

struct type type_long_type =
	{ reduce_long_type, hold_both, drop_both, "long_type" };

struct value value_long_type = { 1, &type_long_type, 0, 0 };
