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

/* Use pattern p to make a copy of f with x substituted in various places.

I do one level of look-ahead on the left and right patterns, which isn't
strictly necessary but is 1% faster and saves 144 bytes.

Checking p->R in addition to p->L is redundant, but is 0.4% faster and saves 80
bytes.
*/
value subst(value p, value f, value x)
	{
	if (p->L && p->R)
		{
		value L = (p->L->T == type_C ? hold(f->L) : subst(p->L,f->L,x));
		value R = (p->R->T == type_C ? hold(f->R) : subst(p->R,f->R,x));
		return A(L,R);
		}
	else if (p->T == type_C)
		return hold(f);
	else
		return hold(x);
	}
