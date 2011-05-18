#include <stdio.h>
#include "value.h"
#include "apply.h"
#include "char_put.h"
#include "I.h"
#include "long.h"

int reduce_char_put(struct value *value)
	{
	if (!value->L) return 0;
	putchar(evaluate_long(value->R));
	replace(value, &value_I);
	return 0;
	}

struct type type_char_put =
	{ reduce_char_put, hold_both, drop_both, "char_put" };
struct value value_char_put = { 1, &type_char_put, 0, 0 };
