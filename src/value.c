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

The N field also serves to link values on the free list.  It is not strictly
portable to store a pointer in an unsigned long field, but people have relied
on that for decades, and it works on every machine I use.

I considered using the uintptr_t type mentioned in the C Reference Standard,
but that type is designated as "optional" and thus not strictly portable.

A union would do the trick, but the ISO C90 standard does not support unnamed
unions, and using a named union is more pain than it's worth.
*/

static value free_list = 0;

static void clear_free_list(void)
	{
	while (free_list)
		{
		value f = free_list;
		free_list = (value)f->N;
		free_memory(f,sizeof(struct value));
		}
	}

void end_value(void)
	{
	clear_free_list();
	end_memory();
	}

static void recycle(value f)
	{
	if (f->L)
		{
		/* Clear pair. */
		drop(f->L);
		drop(f->R);
		}
	else if (f->R)
		{
		/* Clear atom. */
		f->T(f);
		}
	f->N = (unsigned long)free_list;
	free_list = f;
	}

/* Increment the reference count. */
value hold(value f)
	{
	f->N++;
	return f;
	}

/* Decrement the reference count and recycle if it drops to zero. */
void drop(value f)
	{
	if (--f->N == 0) recycle(f);
	}

/* Return a value of type T with the given left and right side. */
static value V(type T, value L, value R)
	{
	value f = free_list;
	if (f)
		free_list = (value)f->N;
	else
		f = (value)new_memory(sizeof(struct value));

	f->N = 1;
	f->T = T;
	f->L = L;
	f->R = R;
	return f;
	}

/* Create a combinator of type T.  Shorthand for "quote". */
value Q(type T)
	{
	return V(T,0,0);
	}

/* Create an atom of type T with data x. */
value D(type T, void *x)
	{
	return V(T,0,x);
	}

/* Evaluate x and apply the result to y. */
value apply(value x, value y)
	{
	x = eval(x);
	return V(x->T,x,y);
	}

value type_A(value f)
	{
	return apply(hold(f->L),hold(f->R));
	}

/* Apply x to y. */
value A(value x, value y)
	{
	return V(type_A,x,y);
	}

/* Reduce the value to its normal form if possible. */
value eval(value f)
	{
	while (1)
		{
		value g = f->T(f);
		if (g == f) break;
		drop(f);
		f = g;
		}
	return f;
	}
