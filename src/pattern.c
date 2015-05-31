#include <value.h>
#include <basic.h>
#include <pattern.h>

/* Make a pattern that ignores the form and returns the argument. */
value none(void) { return Q(type_C); }

/* Make a pattern that returns the form and ignores the argument. */
value here(void) { return Q(type_I); }

/* Make a pattern that sends the argument to the left and right as needed. */
value fuse(value p, value q)
	{
	if (p->T == type_C && q->T == type_C)
		{
		drop(p);
		drop(q);
		return none();
		}
	else
		return A(p,q);
	}

/* Make a copy of f, but substitute x wherever I appears in pattern p. */
value subst(value p, value f, value x)
	{
	if (p->T == type_C)
		return hold(f);
	else if (p->T == type_I)
		return hold(x);
	else
		return A(subst(p->L,f->L,x),subst(p->R,f->R,x));
	}
