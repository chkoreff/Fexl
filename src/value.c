#include <memory.h>
#include <value.h>

/*
The value structure is defined as follows.

The N field is the reference count.

The T field is the type, a C routine which reduces the value during evaluation.

The L and R fields are the left and right components of the value.
If L != 0 and R != 0, the value represents the application of L to R.
If L == 0 and R == 0, the value represents a primary function.
If L == 0 and R != 0, the value represents an atom, and R->R points to some
data.  The reason for the indirection is so I can do inline replacement, but I
plan to eliminate that technique soon.

The N field also serves to link values on the free list.  It is not strictly
portable to store a pointer in an unsigned long field, but people have relied
on that for decades, and it works on every machine I use.

I considered using the uintptr_t type mentioned in the C Reference Standard,
but that type is designated as "optional" and thus not strictly portable.

A union would do the trick, but the ISO C90 standard does not support unnamed
unions, and using a named union is more pain than it's worth.
*/

static value free_list = 0;

static void push_free(value f)
	{
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
	if (--f->N == 0)
		push_free(f);
	}

/* Clear the contents of a value. */
static void clear(value f)
	{
	if (f->L) /* Clear pair. */
		{
		drop(f->L);
		drop(f->R);
		}
	else if (f->R && --f->R->N == 0) /* Clear atom. */
		{
		f->N = 0;
		f->T(f);
		push_free(f->R);
		f->R->R = 0;
		}
	}

/* Pop the first entry off the free list and clear it. */
static value pop_free(void)
	{
	value f = free_list;
	free_list = (value)f->N;
	clear(f);
	return f;
	}

static void clear_free_list(void)
	{
	while (free_list)
		free_memory(pop_free(),sizeof(struct value));
	}

void end_value(void)
	{
	clear_free_list();
	end_memory();
	}

/* Return a value of type T with the given left and right side. */
value V(type T, value L, value R)
	{
	value f = free_list ? pop_free() : new_memory(sizeof(struct value));
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

/* Create an atom of type T with the given data. */
value D(type T, void *data)
	{
	return V(T,0,V(0,0,data));
	}

/* Return the data from an atom. */
void *data(value f)
	{
	return f->R->R;
	}

/* The type for function application */
value type_A(value f)
	{
	value x = arg(f->L);
	if (x != f->L) return V(x->T,x,hold(f->R));
	f->T = x->T;
	drop(x);
	return f;
	}

/* Apply x to y. */
value A(value x, value y)
	{
	return V(type_A,x,y);
	}

/* Replace the content of f with the content of g. */
static value replace(value f, value g)
	{
	clear(f);

	if (g->L) hold(g->L);
	if (g->R) hold(g->R);

	f->T = g->T;
	f->L = g->L;
	f->R = g->R;

	drop(g);
	return f;
	}

/* type_J(f) evaluates the right side first, then if possible replaces f with
the final value.  This ensures proper behavior with the "once" function. */
value type_J(value f)
	{
	value x = f->R;
	while (1)
		{
		value y;
		if (x->T == type_J) /* Avoid stacks of J forms. */
			{
			x = hold(x->R);
			drop(f->R);
			f->R = x;
			continue;
			}

		y = x->T(x);
		if (y == x)
			continue;

		if (y == 0)
			replace(f,hold(x));
		return y;
		}
	}

/* Reduce f to the equivalent of g. */
value reduce(value f, value g)
	{
	clear(f);
	f->T = type_J;
	f->L = Q(type_J);
	f->R = g;
	return f;
	}

/* Reduce f to Q(T). */
value reduce_Q(value f, type T)
	{
	clear(f);
	f->T = T;
	f->L = 0;
	f->R = 0;
	return 0;
	}

/* Reduce f to D(T,data). */
value reduce_D(value f, type T, void *data)
	{
	clear(f);
	f->T = T;
	f->L = 0;
	f->R = V(0,0,data);
	return 0;
	}

/* Reduce f to A(x,y). */
value reduce_A(value f, value x, value y)
	{
	clear(f);
	f->T = type_A;
	f->L = x;
	f->R = y;
	return f;
	}

/* Reduce the value until done. */
value eval(value f)
	{
	while (1)
		{
		value g = f->T(f);
		if (g == 0) return f;
		if (g != f)
			{
			drop(f);
			f = g;
			}
		}
	}

value arg(value f)
	{
	return eval(hold(f));
	}
