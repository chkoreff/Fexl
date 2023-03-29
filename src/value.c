#include <memory.h>
#include <value.h>

/*
The value structure is defined as follows.

The N field is the reference count.

The T field is the type, a C routine which steps the value during evaluation.

The L and R fields are the left and right components of the value, defined as
follows:

1. If L == 0, then R == 0 and the value is a primary function (combinator).

2. If L != 0 and L->N == 0, the value is an atom.  R points to the data, and
L->T points to the function that frees the data when N == 0.

3. If L != 0 and L->N > 0, the value is the function application of L to R.

See the "drop" routine below for a succinct example of those rules.

The N field also serves to link values on the free list.  It is not strictly
portable to store a pointer in an unsigned long field, but people have relied
on that for decades, and it works on every machine I use.

I considered using the uintptr_t type mentioned in the C Reference Standard,
but that type is designated as "optional" and thus not strictly portable.

A union would do the trick, but the ISO C90 standard does not support unnamed
unions, and using a named union is more pain than it's worth.
*/

static value free_list = 0;

/* Recycle a value f with reference count 0, putting f on the free list and
dropping its content.  If f is an atom, its data is freed.  If f is a function
application, its L and R reference counts are decremented and those are also
recycled if their counts reach 0. */
static void recycle(value f)
	{
	value stack = f;
	while (stack)
		{
		value f = stack;
		stack = (value)f->N;

		if (f->L)
			{
			if (f->L->N)
				{
				if (--f->R->N == 0)
					{
					f->R->N = (unsigned long)stack;
					stack = f->R;
					}

				if (--f->L->N == 0)
					{
					f->L->N = (unsigned long)stack;
					stack = f->L;
					}
				}
			else
				{
				void (*free)(value) = (void *)f->L->T;
				free(f);
				}
			}

		f->N = (unsigned long)free_list;
		free_list = f;
		}
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

/* Decrement the reference count and recycle if it drops to zero. */
void drop(value f)
	{
	if (--f->N == 0)
		recycle(f);
	}

void clear_free_list(void)
	{
	while (free_list)
		free_memory(pop_free(),sizeof(struct value));
	}

void end_value(void)
	{
	clear_free_list();
	end_memory();
	}

static value new_value(void)
	{
	return free_list ? pop_free() : new_memory(sizeof(struct value));
	}

/* Return a value of type T with the given left and right side. */
value V(type T, value L, value R)
	{
	value f = new_value();
	*f = (struct value){1, T, L, {.R = R}};
	return f;
	}

// Return a value of type T with a double value.
value V_double(type T, value L, double v_double)
	{
	value f = new_value();
	*f = (struct value){1, T, L, {.v_double = v_double}};
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
