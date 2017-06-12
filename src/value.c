#include <memory.h>
#include <die.h>
#include <value.h>

/*
The value structure is defined as follows.

The N field is the reference count.

The T field is the type, a C routine which steps the value during evaluation.

The L and R fields are the left and right components of the value.
If L != 0 and R != 0, the value represents the application of L to R.
If L == 0 and R == 0, the value represents a primary function.
If L == 0 and R != 0, the value represents an atom, and R points to the data.

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

/* Increment the reference count. */
value hold(value f)
	{
	f->N++;
	return f;
	}

/* Clear the content of a value going on the free list. */
static void clear(value f)
	{
	if (f->L) /* Clear pair. */
		{
		drop(f->L);
		drop(f->R);
		}
	else if (f->R) /* Clear atom. */
		{
		f->N = 0;
		f->T(f);
		}
	f->T = 0;
	f->L = 0;
	f->R = 0;
	}

/* Decrement the reference count and recycle if it drops to zero. */
void drop(value f)
	{
	if (f->N == 0)
		die("XDROP");

	if (--f->N == 0)
		{
		clear(f);
		push_free(f);
		}
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
	#if DEV
	count_V++;
	#endif
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
	value x = arg(f->L);
	return V(x->T,x,hold(f->R));
	}

/* Apply x to y. */
value A(value x, value y)
	{
	return V(type_A,x,y);
	}

#if DEV
unsigned long num_steps = 0;
unsigned long count_V = 0;
#endif

static value step(value f)
	{
	#if DEV
	if (0 && num_steps >= 40000000) return 0;
	num_steps++;
	#endif
	return f->T(f);
	}

/* Reduce the value until done. */
value eval(value f)
	{
	while (1)
		{
		value g = step(f);
		if (g == 0) return f;
		drop(f);
		f = g;
		}
	}

value arg(value f)
	{
	return eval(hold(f));
	}
