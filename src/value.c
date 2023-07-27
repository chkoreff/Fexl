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

The "recycle" routine below succinctly reflects these rules.

Note that on most machines, (sizeof(struct value) == 32).
*/

static value free_list = 0;

static value new_value(void)
	{
	value f = free_list;
	if (f)
		free_list = f->next;
	else
		f = new_memory(sizeof(struct value));
	return f;
	}

// Recycle a value f with reference count 0, putting f on the free list and
// dropping its content.  If f is data, its data is freed.  If f is a tree, its
// L and R reference counts are decremented and those are also recycled if
// their counts reach 0.
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

// Increment the reference count.
value hold(value f)
	{
	f->N++;
	return f;
	}

// Decrement the reference count and recycle if it drops to zero.
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

// Create data of type T with double value x.
value Qdouble(type T, value L, double x)
	{
	value f = new_value();
	f->N = 1;
	f->T = T;
	f->L = L;
	f->v_double = x;
	return f;
	}

// Apply x to y where x is known to be already evaluated.
value AV(value x, value y)
	{
	return V(x->T,x,y);
	}

// The type for function application
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

// Apply x to y.
value A(value x, value y)
	{
	return V(type_A,x,y);
	}

unsigned long num_steps = 0;

// Reduce the value until done.
value eval(value f)
	{
	while (1)
		{
		value g = f->T(f);
		if (g == 0) return f;
		num_steps++;
		drop(f);
		f = g;
		}
	}

value arg(value f)
	{
	return eval(hold(f));
	}
