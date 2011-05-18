#include "value.h"
#include "long.h"

void hold_long(struct value *value)
	{
	if (value->L)
		hold_both(value);
	else
		{
		struct atom_long *atom = (struct atom_long *)value->R;
		atom->N++;
		}
	}

void drop_long(struct value *value)
	{
	if (value->L)
		drop_both(value);
	else
		{
		struct atom_long *atom = (struct atom_long *)value->R;
		if (--atom->N <= 0)
			free_memory(atom, sizeof(struct atom_long));
		}
	}

struct type type_long = { reduce_none, hold_long, drop_long, "long" };

/* Make a new value of type long.  */
struct value *new_long(long number)
	{
	struct atom_long *atom = new_memory(sizeof(struct atom_long));
	atom->N = 0;
	atom->data = number;

	return new_value(&type_long, 0, (struct value *)atom);
	}

/* Evaluate a value of type long. */
long evaluate_long(struct value *value)
	{
	evaluate_type(value, &type_long);
	struct atom_long *atom = (struct atom_long *)value->R;
	return atom->data;
	}
