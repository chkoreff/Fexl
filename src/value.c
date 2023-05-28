#include <memory.h>
#include <value.h>

static value free_list = 0;

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
		{
		f->T->clear(f);
		f->next = free_list;
		free_list = f;
		}
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

static value new_value(type T, value L)
	{
	value f = free_list;
	if (f)
		free_list = f->next;
	else
		f = new_memory(sizeof(struct value));

	f->N = 1;
	f->T = T;
	f->L = L;
	return f;
	}

// Return a value of type T with the given left and right side.
value V(type T, value L, value R)
	{
	value f = new_value(T,L);
	f->R = R;
	return f;
	}

// Create an atom of type T with data pointer p.
value Q(type T, void *p)
	{
	value f = new_value(T,0);
	f->v_ptr = p;
	return f;
	}

// Create an atom of type T with double value x.
value Qdouble(type T, double x)
	{
	value f = new_value(T,0);
	f->v_double = x;
	return f;
	}

// The type for function application
value step_A(value f)
	{
	value g = eval(hold(f->L));
	g = g->T->apply(g,hold(f->R));
	drop(f);
	return g;
	}

void clear_A(value f)
	{
	drop(f->L);
	drop(f->R);
	}

struct type type_A = { step_A, 0, clear_A };

// Apply x to y.
value A(value x, value y)
	{
	return V(&type_A,x,y);
	}

void no_clear(value f)
	{
	(void)f;
	}

// Nullary functions

static value step_Z(value f)
	{
	value (*op)(void) = f->v_ptr;
	value g = op();
	drop(f);
	return g;
	}

struct type type_Z = { step_Z, 0, no_clear };

// Reduce the value until done.
value eval(value f)
	{
	while (f->T->step)
		f = f->T->step(f);
	return f;
	}
