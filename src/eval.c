#include "value.h"
#include "die.h"
#include "eval.h"
#include "run.h"
#include "stack.h"

/*
Evaluate a value, reducing it to a final normal form if possible within any
limits on time and space.

When eval reduces the left side of a function application, it does not call
itself recursively using the system stack.  Instead, it uses its own stack in
the form of a linked list of values.  This allows it to reduce values with very
deep left sides without causing a segmentation fault.
*/
void eval(struct value *f)
	{
	push(f);

	while (stack)
		{
		if (max_steps >= 0 && ++cur_steps > max_steps)
			die("Your program ran too long");

		f = stack->L;

		if (f->T)
			pop();
		else if (f->L->T)
			{
			struct value *g = f->L->T(f);
			if (g && g != f)
				replace(f,g);
			else if (f == stack->L)
				f->T = f->L->T;
			}
		else
			push(f->L);
		}
	}
