#include "value.h"
#include "string.h"
#include "type.h"

/* Return true if the argument is of type string.  */
int reduce_string_type(struct value *value)
	{
	return reduce_type_check(value, &type_string);
	}

struct type type_string_type =
	{ reduce_string_type, hold_both, drop_both, "string_type" };

struct value value_string_type = { 1, &type_string_type, 0, 0 };
