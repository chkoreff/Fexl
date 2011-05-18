#include <stdio.h>
#include <stdlib.h>
#include "value.h"
#include "string.h"
#include "var.h"

int reduce_var(struct value *value)
	{
	if (!value->L)
		{
		struct atom_string *atom = (struct atom_string *)value->R;
		fprintf(stderr, "Attempt to call undefined var %s\n", atom->data);
		exit(1);
		}
	return 0;
	}

struct type type_var = { reduce_var, hold_string, drop_string, "var" };
