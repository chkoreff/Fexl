#include "value.h"
#include "apply.h"

int reduce_apply(struct value *value)
	{
	evaluate(value->L);
	value->T = value->L->T;
	return 1;
	}

struct type type_apply = { reduce_apply, hold_both, drop_both, "apply" };

struct value *apply(struct value *fun, struct value *arg)
	{
	return new_value(&type_apply, fun, arg);
	}
