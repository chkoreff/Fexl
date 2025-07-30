#include <memory.h>
#include <value.h>

/*
The value structure is defined as follows.

Let f be a pointer to a struct value.

f->N is the reference count.

f->T is the type, a C routine which reduces the value during evaluation.

f->next links values on the free list after f->N drops to 0.

Every value f is one of three classes: atom, data, or tree.

1. atom : (f->L == 0 && f->R == 0)

	An atom value is a primary function with no arguments.

2. data : (f->L != 0 && f->L->N == 0)

	A data value has data that resides in the union which follows f->L.  The
	f->L->clear field is a function that frees the data when f->N drops to 0.

3. tree : (f->L != 0 && f->L->N > 0)

	A tree value is a combination of values f->L and f->R.

The "discard" routine below succinctly reflects these rules.

Note that on most machines, (sizeof(struct value) == 32).
*/

static value free_list = 0;

value new_value(void)
	{
	value f = free_list;
	if (f)
		free_list = f->next;
	else
		f = new_memory(sizeof(struct value));
	return f;
	}

void recycle(value f)
	{
	f->next = free_list;
	free_list = f;
	}

// Discard a value f with reference count 0.  This drops the L and R fields of
// a tree value, or calls the clear routine of a data value, and recycles it.
static void discard(value f)
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
	recycle(f);
	}

// Increment the reference count.
value hold(value f)
	{
	f->N++;
	return f;
	}

// Decrement the reference count and discard if it drops to zero.
void drop(value f)
	{
	if (--f->N == 0)
		discard(f);
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

// Return a value of type T with the given left and right side.
value V(type T, value L, value R)
	{
	value f = new_value();
	f->N = 1;
	f->T = T;
	f->L = L;
	f->R = R;
	return f;
	}

// Create an atom of type T.
value Q(type T)
	{
	return V(T,0,0);
	}

// Apply x to y.
value A(value x, value y)
	{
	return V(0,x,y);
	}

value keep(value f)
	{
	f->T = f->L->T;
	return hold(f);
	}

value tail(value f)
	{
	value x = hold(f->R);
	drop(f);
	return x;
	}

unsigned long cur_steps;

static value step(value f)
	{
	value g = f->L;
	if (g->T == 0)
		{
		if (f->N == 1)
			g = (f->L = eval(g));
		else
			{
			g = eval(hold(g));
			if (g == f->L)
				drop(g);
			else
				f = A(g,tail(f));
			}
		}

	g = g->T(f);
	drop(f);
	cur_steps++;
	return g;
	}

// Reduce the value until done.
value eval(value f)
	{
	while (f->T == 0)
		f = step(f);
	return f;
	}

value arg(value f)
	{
	return eval(hold(f));
	}
