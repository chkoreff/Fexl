#include <stdio.h>
#include "value.h"
#include "apply.h"
#include "char_get.h"
#include "long.h"

int reduce_char_get(struct value *value)
	{
	if (!value->L) return 0;
	replace(value, apply(value->R,new_long(getchar())));
	return 1;
	}

struct type type_char_get =
	{ reduce_char_get, hold_both, drop_both, "char_get" };
struct value value_char_get = { 1, &type_char_get, 0, 0 };
