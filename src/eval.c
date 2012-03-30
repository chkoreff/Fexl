#include "value.h"
#include "die.h"
#include "eval.h"
#include "run.h"

int arg(type T, value x)
	{
	if (x->T == 0) eval(x);
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
*/
void eval(value f)
	{
	if (++cur_depth > max_depth)
		die("Your program is too deeply nested.");

	while (1)
		{
		if (f->T) break;

		if (max_steps >= 0 && ++cur_steps > max_steps)
			die("Your program ran too long");

		value x = f->L;
		if (x->T == 0) eval(x);

		value g = x->T(f);
		if (g && g != f)
			replace(f,g);
		else
			f->T = x->T;
		}

	cur_depth--;
	}
