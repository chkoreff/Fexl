#include "memory.h"
#include "value.h"

static value free_list = 0;

/* Put the value on the free list. */
static void recycle(value f)
	{
	f->N = (long)free_list;
	free_list = f;
	}

/* Increment the reference count. */
void hold(value f)
	{
	f->N++;
	}

/*
Decrement the reference count and put the value on the free list if it drops to
zero.  We are very strict about this.  It must be exactly zero, not negative.
A negative reference count indicates an imbalance in hold/drop calls, which is
sloppy programming, and in that case we want to report a memory leak error when
the program ends.
*/
void drop(value f)
	{
	if (--f->N == 0) recycle(f);
	}

/* Return a value of type T with the given left and right side. */
value V(type T, value L, value R)
	{
	if (L)
		{
		hold(L);
		hold(R);
		}

	value f = free_list;
	if (f)
		{
		free_list = (value)f->N;
		if (f->L)
			{
			/* Clear pair. */
			drop(f->L);
			drop(f->R);
			}
		else if (f->R)
			{
			/* Clear atom. */
			f->N = 0;
			f->T(f);
			}
		}
	else
		f = (value)new_memory(sizeof (struct value));

	f->N = 0;
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

static value type_apply(value f)
	{
	value g = eval(f->L);
	value z = V(g->T, g, f->R);
	drop(g);
	return z;
	}

/* Return a value which is f applied to g. */
value A(value f, value g)
	{
	return V(type_apply,f,g);
	}

/* Evaluate the value, returning its normal form if possible within any limits
on space and time. */
value eval(value f)
	{
	while (1)
		{
		hold(f);
		value g = f->T(f);
		if (g == 0) return f;
		drop(f);
		f = g;
		}
	}

/* Clear the free list and end the memory arena. */
void end_value(void)
	{
	while (free_list)
		free_memory(Q(0), sizeof(struct value));

	end_memory();
	}
