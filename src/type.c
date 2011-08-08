#include "value.h"
#include "apply.h"
#include "C.h"
#include "I.h"
#include "type.h"

/* Return T if the argument has the given type, otherwise F.  */
int reduce_type_check(struct value *value, struct type *type)
	{
	if (!value->L) return 0;

	struct value *arg = value->R;
	evaluate(arg);

	if (!arg->L && arg->T == type)
		replace(value, &value_C);
	else
		replace(value, apply(&value_C,&value_I));

	return 0;
	}
