#include <die.h>
#include <limits.h>
#include <memory.h>
#include <value.h>

/*
The value structure is defined as follows.

The N field is the reference count.

The T field is the type, a C routine which reduces the value during evaluation.
This routine also serves to clear an atom whose reference count drops to 0.

The L and R fields are the left and right components of the value.
If L == 0, the value is an atom, and R points to the atom data, if any.
If L != 0, then R != 0, and the value represents the application of L to R.
*/

/* Free the value so it can be reused. */
static void recycle(value f)
	{
	if (f->L)
		{
		/* Clear pair. */
		drop(f->L);
		drop(f->R);
		}
	else if (f->R)
		/* Clear atom. */
		f->T(f);

	free_memory(f, sizeof(struct value));
	}

/* Increment the reference count. */
value hold(value f)
	{
	if (f->N == ULONG_MAX) die("hold");
	f->N++;
	return f;
	}

/* Decrement the reference count and recycle if it drops to zero. */
void drop(value f)
	{
	if (f->N == 0) die("drop");
	if (--f->N == 0) recycle(f);
	}

/* Return a value of type T with the given left and right side. */
value V(type T, value L, value R)
	{
	value f = (value)new_memory(sizeof (struct value));
	f->N = 1;
	f->T = T;
	f->L = L;
	f->R = R;
	return f;
	}

/* Return a data value with the given pointer. */
value D(type T, void *p)
	{
	return V(T,0,p);
	}

/* Create a combinator of type T.  Shorthand for "quote". */
value Q(type T)
	{
	return D(T,0);
	}

void bad_type(void)
	{
	die("You used a data type incorrectly.");
	}

/* Return the data pointer of an atom of type t. */
void *get_data(value f, type t)
	{
	if (f->L != 0 || f->T != t) bad_type();
	return f->R;
	}

/* Apply f to x eagerly. */
value apply(value f, value x)
	{
	f = eval(f);
	return V(f->T,f,x);
	}

value type_A(value f)
	{
	return apply(hold(f->L),hold(f->R));
	}

/* Apply f to x lazily. */
value A(value f, value x)
	{
	return V(type_A,f,x);
	}

/* Evaluate the value, returning its normal form if possible within any limits
on space and time. */
value eval(value f)
	{
	while (1)
		{
		value g = f->T(f);
		if (g == 0) return f;
		drop(f);
		f = g;
		}
	}

/* Evaluate an argument inside a form. */
value arg(value f)
	{
	return eval(hold(f));
	}
