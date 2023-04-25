#include <memory.h>
#include <value.h>

/*
The value structure is defined as follows.

Let f be a pointer to a struct value.

f->N is the reference count.

f->T is the type, a C routine which steps the value during evaluation.

f->next links values on the free list after f->N drops to 0.

Every value f is one of three classes: combinator, atom, or apply.

1. combinator : (f->L == 0 && f->R == 0)

A combinator is a primary function with no arguments.

2. atom       : (f->L != 0 && f->L->N == 0)

An atom is a piece of data.  The data reside in the union which follows f->L.
The f->L->clear field is a function that frees the data when f->N drops to 0.

3. apply      : (f->L != 0 && f->L->N > 0)

An apply is the application of f->L to f->R.

The "recycle" routine below succinctly reflects these rules.

Note that on most machines, (sizeof(struct value) == 32).
*/

static value free_list = 0;

// Recycle a value f with reference count 0, putting f on the free list and
// dropping its content.  If f is an atom, its data is freed.  If f is an
// apply, its L and R reference counts are decremented and those are also
// recycled if their counts reach 0.
static void recycle(value f)
	{
	if (f->L)
		{
		if (f->L->N)
			{
			drop(f->L);
			drop(f->R);
			}
		else
			f->L->clear(f);
		}

	f->next = free_list;
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
		recycle(f);
	}

void clear_free_list(void)
	{
	while (free_list)
		{
		value f = free_list;
		free_list = f->next;
		free_memory(f,sizeof(struct value));
		}
	}

void end_value(void)
	{
	clear_free_list();
	end_memory();
	}

static value new_value(void)
	{
	value f = free_list;
	if (f)
		{
		free_list = f->next;
		return f;
		}
	return new_memory(sizeof(struct value));
	}

/* Return a value of type T with the given left and right side. */
value V(type T, value L, value R)
	{
	value f = new_value();
	*f = (struct value){{.N=1}, {.T=T}, L, {.R=R}};
	return f;
	}

// Return a value of type T with a double value.
value V_double(type T, value L, double v_double)
	{
	value f = new_value();
	*f = (struct value){{.N=1}, {.T=T}, L, {.v_double = v_double}};
	return f;
	}

/* Create a combinator of type T.  Shorthand for "quote". */
value Q(type T)
	{
	return V(T,0,0);
	}

/* Apply x to y where x is known to be already evaluated. */
value AV(value x, value y)
	{
	return V(x->T,x,y);
	}

/* The type for function application */
value type_A(value f)
	{
	value x = arg(f->L);
	if (x == f->L)
		{
		f->T = x->T;
		drop(x);
		return hold(f);
		}
	else
		return AV(x,hold(f->R));
	}

/* Apply x to y. */
value A(value x, value y)
	{
	return V(type_A,x,y);
	}

/* Reduce the value until done. */
static value eval_normal(value f)
	{
	while (1)
		{
		value g = f->T(f);
		if (g == 0) return f;
		drop(f);
		f = g;
		}
	}

value (*eval)(value f) = eval_normal;

value arg(value f)
	{
	return eval(hold(f));
	}
