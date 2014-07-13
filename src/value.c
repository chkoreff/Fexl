#include <die.h>
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

static value free_list = 0;

static void clear_free_list(void)
	{
	while (free_list)
		{
		value f = free_list;
		free_list = f->L;
		free_memory(f, sizeof(struct value));
		}
	}

void end_value(void)
	{
	clear_free_list();
	end_memory();
	}

/* Free the value so it can be reused. */
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
		f->R->R = 0;
		recycle(f->R);
		}

	f->L = free_list;
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
	if (f->N == 0) die("drop");
	if (--f->N == 0) recycle(f);
	}

/* Return a value of type T with the given left and right side. */
value V(type T, value L, value R)
	{
	value f = free_list;
	if (f)
		free_list = f->L;
	else
		f = (value)new_memory(sizeof (struct value));

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

/* Create a data item of type T, with the given free function and value. */
value D(type T, type free, value p)
	{
	return V(T,0,V(free,0,p));
	}

/* Apply f to x. */
value A(value f, value x)
	{
	return V(0,f,x);
	}

/* Return the data pointer of an atom of type t. */
value get_D(value f, type t)
	{
	if (f->T == t && f->L == 0)
		return f->R->R;
	else
		return 0;
	}

/* Replace f with V(T,L,R). */
void replace_V(value f, type T, value L, value R)
	{
	drop(f->L);
	drop(f->R);
	f->T = T;
	f->L = L;
	f->R = R;
	}

/* Replace f with a data item. */
void replace_D(value f, type T, type free, value p)
	{
	replace_V(f,T,0,V(free,0,p));
	}

/* Replace f with g. */
void replace(value f, value g)
	{
	if (g->L) hold(g->L);
	if (g->R) hold(g->R);
	replace_V(f,g->T,g->L,g->R);
	}

/* Replace f with A(L,R). */
void replace_A(value f, value L, value R)
	{
	replace_V(f,0,L,R);
	}

/* Reduce the value to its normal form if possible within any limits on space
and time. */

unsigned long cur_depth = 0;
unsigned long max_depth = 100000;

unsigned long cur_steps = 0;
unsigned long max_steps = 100000000;

void out_of_stack(void)
	{
	die("out of stack");
	}

void out_of_time(void)
	{
	die("out of time");
	}

value eval(value f)
	{
	if (++cur_depth > max_depth) out_of_stack();
	while (f->T == 0)
		{
		if (++cur_steps > max_steps) out_of_time();
		(f->T = eval(f->L)->T)(f);
		}
	cur_depth--;
	return f;
	}
