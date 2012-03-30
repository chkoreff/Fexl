#include "value.h"
#include "die.h"
#include "eval.h"
#include "run.h"

/* The stack used for tracking recursive goals during evaluation. */
static value stack = 0;

int arg(type T, value x)
	{
	if (x->T == 0)
		{
		push(&stack, x);
		return 0;
		}

	return x->T == T && !x->L;
	}

/* Used for functions of the form (is_T x yes no), which checks the type of x
and returns yes if it's type T or no otherwise. */
value arg_is_type(type T, value f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	value x = f->L->L->R;
	if (arg(T,x)) return f->L->R;
	if (x->T == 0) return f;
	return f->R;
	}

/*
Evaluate a value, reducing it to a final normal form if possible within any
limits on time and space.

When eval reduces the left side of a function application, it does not call
itself recursively using the system stack.  Instead, it uses its own stack in
the form of a linked list of values.  This allows it to reduce values with very
deep left sides without causing a segmentation fault.
*/
void eval(value f)
	{
	hold(f);
	push(&stack, f);

	while (stack)
		{
		if (max_steps >= 0 && ++cur_steps > max_steps)
			die("Your program ran too long");

		f = stack->L;
		value x = f->L;

		if (f->T)
			pop(&stack);
		else if (x->T)
			{
			value g = x->T(f);
			if (g && g != f)
				replace(f,g);
			else if (f == stack->L)
				f->T = x->T;
			}
		else
			push(&stack, x);
		}

	drop(f);
	}
