#include "value.h"
#include "memory.h"

value free_list = 0;

/* Put the value on the free list. */
static void recycle(value f)
	{
	f->N = (long)free_list;
	free_list = f;
	}

/* Increment the reference count. */
void hold(value f)
	{
	if (f) f->N++;
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
	if (f && --f->N == 0) recycle(f);
	}

static void clear(value f)
	{
	if (f->L)
		{
		/* Clear a pair. */
		if (--f->L->N == 0)
			recycle(f->L);
		if (f->R && --f->R->N == 0)
			recycle(f->R);
		}
	else
		{
		/* Clear an atom. */
		if (f->R && --f->R->N == 0)
			{
			if (f->R->T)
				f->R->T(f); /* Call the clear routine for the data. */
			f->R->L = 0;
			f->R->R = 0;
			recycle(f->R);
			}
		}
	}

/* Create a new value. */
static value create(type T, value L, value R)
	{
	value f = free_list;
	if (f)
		{
		free_list = (value)f->N;
		clear(f);
		}
	else
		f = (value)new_memory(sizeof (struct value));

	f->N = 0;
	f->T = T;
	f->L = L;
	f->R = R;

	if (L) L->N++;
	if (R) R->N++;

	return f;
	}

/* Create a data object of type T. */
value D(type T)
	{
	return create(T,0,create(0,0,0));
	}

/* Create a combinator of type T.  Shorthand for "quote". */
value Q(type T)
	{
	return create(T,0,0);
	}

/* Create a function which applies L to R. */
value A(value L, value R)
	{
	return create(0,L,R);
	}

/* Replace the contents of f with the contents of g.  Assumes g && f != g. */
void replace(value f, value g)
	{
	g->N++;

	clear(f);

	f->T = g->T;
	f->L = g->L;
	f->R = g->R;

	if (f->L) f->L->N++;
	if (f->R) f->R->N++;

	if (--g->N == 0)
		recycle(g);
	}

/* Clear the free list and end the memory arena. */
void end_value(void)
	{
	while (free_list)
		free_memory(Q(0), sizeof(struct value));

	end_memory();
	}
