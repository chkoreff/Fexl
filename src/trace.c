#include <stdio.h>
#include <stdlib.h>
#include "value.h"
#include "double.h"
#include "long.h"
#include "string.h"
#include "var.h"

/*
This module is only used when you need to trace values during development or
testing.
*/

void nl(void)
	{
	fputs("\n",stdout);
	}

void print_value(struct value *value)
	{
	if (max_steps >= 0 && ++total_steps > max_steps)
		{
		printf("...\n");
		exit(1);
		}

	if (value->L)
		{
		fputs("(",stdout);
		print_value(value->L);
		fputs(" ",stdout);
		print_value(value->R);
		fputs(")",stdout);
		}
	else
		{
		if (value->T == &type_string || value->T == &type_var)
			{
			fputs(value->T->name, stdout);
			fputs(":",stdout);
			struct atom_string *atom = (struct atom_string *)value->R;
			fputs(atom->data, stdout);
			}
		else if (value->T == &type_double)
			{
			fputs(value->T->name, stdout);
			fputs(":",stdout);
			struct atom_double *atom = (struct atom_double *)value->R;
			printf("%.16g", atom->data);
			}
		else if (value->T == &type_long)
			{
			fputs(value->T->name, stdout);
			fputs(":",stdout);
			struct atom_long *atom = (struct atom_long *)value->R;
			printf("%ld", atom->data);
			}
		else
			{
			fputs(value->T->name, stdout);
			}
		}
	}
