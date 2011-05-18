#include "value.h"
#include "double.h"

void hold_double(struct value *value)
	{
	if (value->L)
		hold_both(value);
	else
		{
		struct atom_double *atom = (struct atom_double *)value->R;
		atom->N++;
		}
	}

void drop_double(struct value *value)
	{
	if (value->L)
		drop_both(value);
	else
		{
		struct atom_double *atom = (struct atom_double *)value->R;
		if (--atom->N <= 0)
			free_memory(atom, sizeof(struct atom_double));
		}
	}

struct type type_double = { reduce_none, hold_double, drop_double, "double" };

/* Make a new double-precision floating point number.  */
struct value *new_double(double number)
	{
	struct atom_double *atom = new_memory(sizeof(struct atom_double));
	atom->N = 0;
	atom->data = number;

	return new_value(&type_double, 0, (struct value *)atom);
	}

/* Evaluate a value of type double. */
double evaluate_double(struct value *value)
	{
	evaluate_type(value, &type_double);
	struct atom_double *atom = (struct atom_double *)value->R;
	return atom->data;
	}
