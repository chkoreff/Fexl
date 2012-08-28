#include "die.h"
#include "memory.h"
#include "value.h"

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

/* Clear a value prior to reuse. */
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

/* Create a function that applies L to R. */
value A(value L, value R)
	{
	return create(0,L,R);
	}

/* Replace the contents of f with the contents of g. */
void replace(value f, value g)
	{
	g->N++;

	value g0 = g->L;
	value g1 = g->R;

	if (g0) g0->N++;
	if (g1) g1->N++;

	clear(f);

	f->T = g->T;
	f->L = g0;
	f->R = g1;

	if (--g->N == 0) recycle(g);
	}

/* Equivalent to replace(f,A(g0,G1)). */
void replace_apply(value f, value g0, value g1)
	{
	g0->N++;
	g1->N++;

	clear(f);

	f->L = g0;
	f->R = g1;
	}

/*
Evaluate a value, reducing it to a final normal form if possible within any
limits on time and space.
*/
void eval(value f)
	{
	while (f->T == 0)
		{
		eval(f->L);
		f->L->T(f);
		}
	}

/* Determine if a function argument has the correct type. */
int if_type(type T, value x)
	{
	eval(x);
	return x->T == T;
	}

void type_error(void)
	{
	die("You used a data type incorrectly.");
	}

value arg(type T, value x)
	{
	eval(x);
	if (x->T != T) type_error();
	return x;
	}

/* Clear the free list and end the memory arena. */
void end_value(void)
	{
	while (free_list)
		free_memory(Q(0), sizeof(struct value));

	end_memory();
	}
