#include <memory.h>
#include <value.h>

/*
The value structure is defined as follows.

The N field is the reference count.

The T field is the type, a C routine which reduces the value during evaluation.

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

/* Clear the contents of a value. */
static void clear(value f)
	{
	if (f->L) /* Clear pair. */
		{
		drop(f->L);
		drop(f->R);
		}
	else if (f->R) /* Clear atom. */
		f->T(f);
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
	if (--f->N == 0)
		recycle(f);
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
	return V(T,0,data);
	}

value type_H(value f)
	{
	value x = argp(&f->R);
	if (x != f->R) return x;
	return reduce(f,x);
	}

static value H(value x)
	{
	if (x->T == type_H)
		return hold(x);
	else
		return V(type_H,hold(&QI),hold(x));
	}

/* The type for function application */
value type_A(value f)
	{
	value x = argp(&f->L);
	if (x != f->L) return V(x->T,x,H(f->R));
	f->T = x->T;
	drop(x);
	return f;
	}

/* Apply x to y. */
value A(value x, value y)
	{
	return V(type_A,x,y);
	}

/* (J x) Steps the right side until done or an action occurs. */
value type_J(value f)
	{
	value x = f->R;
	while (1)
		{
		if (x->T == type_J) /* Avoid stacks of J forms. */
			{
			x = hold(x->R);
			drop(f->R);
			f->R = x;
			}
		else
			{
			value y = x->T(x);
			if (y != x) return y;
			}
		}
	}

/* (I x) = x */
value type_I(value f)
	{
	if (!f->L) return 0;
	f->T = type_J;
	return f;
	}

struct value QI = { 1, type_I };

/* Reduce f to the equivalent of g. */
value reduce(value f, value g)
	{
	clear(f);
	f->T = type_J;
	f->L = hold(&QI);
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
	f->R = data;
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
		if (g == 0) break;
		if (g != f)
			{
			drop(f);
			f = g;
			}
		}

	if (f->T == type_J)
		{
		value g = hold(f->R);
		drop(f);
		f = g;
		}
	return f;
	}

value arg(value f)
	{
	return eval(hold(f));
	}

value argp(value *p)
	{
	value f = *p;
	value g = arg(f);
	if (f->T == type_J && g == f->R)
		{
		drop(f);
		*p = hold(g);
		}
	return g;
	}
