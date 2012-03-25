#include "value.h"
#include "memory.h"

value free_list = 0;

/* Create a new uninitialized value. */
value create(void)
	{
	value f = free_list;
	if (f)
		{
		free_list = (value)f->N;
		if (f->L)
			{
			drop(f->L);
			drop(f->R);
			}
		else if (f->R && --f->R->N == 0)
			{
			f->R->T(f);
			f->R->L = 0;
			f->R->R = 0;
			recycle(f->R);
			}
		}
	else
		f = (value)new_memory(sizeof (struct value));

	return f;
	}

/* Put the value on the free list. */
void recycle(value f)
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

/* Replace the contents of f with the contents of g.  Note that f and g must
not be equal. */
void replace(value f, value g)
	{
	hold(g);

	drop(f->L);
	drop(f->R);

	f->T = g->T;
	f->L = g->L;
	f->R = g->R;

	if (f->L) hold(f->L);
	if (f->R) hold(f->R);

	drop(g);
	}

/* Create a combinator of type T.  Shorthand for "quote". */
value Q(type T)
	{
	value f = create();
	f->N = 0;
	f->T = T;
	f->L = 0;
	f->R = 0;
	return f;
	}

/* Create a function which applies L to R. */
value A(value L, value R)
	{
	hold(L);
	hold(R);

	value f = create();
	f->N = 0;
	f->T = 0;
	f->L = L;
	f->R = R;

	return f;
	}

/* Clear the free list and end the memory arena. */
void end_value(void)
	{
	while (free_list)
		free_memory(create(), sizeof(struct value));

	end_memory();
	}
