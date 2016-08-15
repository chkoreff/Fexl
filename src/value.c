#include <memory.h>
#include <value.h>

/*
The value structure is defined as follows.

The N field is the reference count.

The T field is the type, a C routine which reduces the value during evaluation.

The L and R fields are the left and right components of the value.
If L != 0 and R != 0, the value represents the application of L to R.
If L == 0 and R == 0, the value represents a primary function.
If L == 0 and R != 0, the value represents an atom with data R.

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

static value pop_free(void)
	{
	value f = free_list;
	free_list = (value)f->N;
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

static void clear(value f)
	{
	if (f->L)
		{
		/* Clear pair. */
		drop(f->L);
		drop(f->R);
		}
	else if (f->R)
		{
		/* Clear atom. */
		f->T(f);
		}
	}

static void recycle(value f)
	{
	clear(f);
	push_free(f);
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
	if (--f->N == 0) recycle(f);
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
	return V(T,0,data);
	}

/* The type for function application */
value type_A(value f)
	{
	value g = next_action(&f->L);
	if (g)
		return A(g,hold(f->R));
	f->T = f->L->T;
	return f;
	}

/* Apply x to y. */
value A(value x, value y)
	{
	return V(type_A,x,y);
	}

/* (J g) = g.  The reduce routine uses type_J to make f equivalent to g without
having to copy the components of g into f.  */
value type_J(value f)
	{
	return hold(f->R);
	}

/* Reduce f to the equivalent of g. */
void reduce(value f, value g)
	{
	clear(f);
	f->T = type_J;
	f->L = Q(type_J);
	f->R = g;
	}

/* Reduce f to Q(T). */
void reduce_Q(value f, type T)
	{
	clear(f);
	f->T = T;
	f->L = 0;
	f->R = 0;
	}

/* Reduce f to D(T,data). */
void reduce_D(value f, type T, void *data)
	{
	clear(f);
	f->T = T;
	f->L = 0;
	f->R = data;
	}

/* Reduce f to A(x,y). */
void reduce_A(value f, value x, value y)
	{
	clear(f);
	f->T = type_A;
	f->L = x;
	f->R = y;
	}

/*
A type function sets the action flag true to indicate a repeatable side-effect.
Otherwise the value would be regarded as a pure function and the side-effect
would only happen once.
*/
int action;

/* Evaluate up to the first action, if any, and replace along the way. */
value next_action(value *p)
	{
	value f = *p;
	action = 0;
	while (1)
		{
		value g = f->T(f);
		if (g == 0) return 0;
		if (action) return g;
		if (g != f)
			{
			drop(f);
			f = g;
			*p = f;
			}
		}
	}

/* Reduce the value until done. */
value eval(value f)
	{
	while (1)
		{
		value g = next_action(&f);
		if (g == 0) return f;
		drop(f);
		f = g;
		}
	}

value arg(value f)
	{
	return eval(hold(f));
	}
