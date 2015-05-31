#include <memory.h>
#include <value.h>

/*
The value structure is defined as follows.

The N field is the reference count.

The T field is the type, a C routine which reduces the value during evaluation.

The L and R fields are the left and right components of the value.
If L != 0 and R != 0, the value represents the application of L to R.
If L == 0 and R == 0, the value represents a primary function.
If L == 0 and R != 0, the value represents an atom.  In that case, R points to
another value which holds a destroy function in T, and a pointer to the atom
data in R.

The N field also serves to link values on the free list.  It is not strictly
portable to store a pointer in an unsigned long field, but people have relied
on that for decades, and it works on every machine I use.

I considered using the uintptr_t type mentioned in the C Reference Standard,
but that type is designated as "optional" and thus not strictly portable.

A union would do the trick, but the ISO C90 standard does not support unnamed
unions, and using a named union is more pain than it's worth.
*/

/*LATER perhaps limit size of free list, and free values when too many */

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
	else if (f->R && --f->R->N == 0)
		{
		/* Clear atom. */
		f->R->T(f->R->R);
		f->R->N = (unsigned long)free_list;
		free_list = f->R;
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

/* Create an atom of type T with the given data and destroy routine. */
value D(type T, void *data, type destroy)
	{
	return V(T,0,V(destroy,0,data));
	}

/* Return the data from an atom. */
void *data(value f)
	{
	return f->R->R;
	}

/* The type for function application */
static value type_A(value f)
	{
	value g = eval(hold(f->L));
	if (g != f->L)
		return V(g->T,g,hold(f->R));

	drop(g);
	f->T = f->L->T;
	return f;
	}

/* Apply x to y. */
value A(value x, value y)
	{
	return V(type_A,x,y);
	}

/* Replace the content of f (non-atomic) with the content of g. */
void replace(value f, value g)
	{
	drop(f->L);
	drop(f->R);

	if (g->L) hold(g->L);
	if (g->R) hold(g->R);

	f->T = g->T;
	f->L = g->L;
	f->R = g->R;

	drop(g);
	}

/* Equivalent to replace(f,Q(t)) */
void replace_Q(value f, type t)
	{
	drop(f->L);
	drop(f->R);

	f->T = t;
	f->L = 0;
	f->R = 0;
	}

/* Equivalent to replace(f,D(T,data,destroy)) */
void replace_D(value f, type T, void *data, type destroy)
	{
	drop(f->L);
	drop(f->R);

	f->T = T;
	f->L = 0;
	f->R = V(destroy,0,data);
	}

/* Equivalent to replace(f,A(x,y)) */
void replace_A(value f, value x, value y)
	{
	drop(f->L);
	drop(f->R);

	f->T = type_A;
	f->L = x;
	f->R = y;
	}

/* Reduce the value to its normal form if possible. */
value eval(value f)
	{
	while (1)
		{
		value g = f->T(f);
		if (g == 0) return f;
		if (g == f) continue;
		drop(f);
		f = g;
		}
	}
