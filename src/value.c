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

The N field also serves to link values on the free list.  It is not strictly
portable to store a pointer in an unsigned long field, but people have relied
on that for decades, and it works on every machine I use.

I considered using the uintptr_t type mentioned in the C Reference Standard,
but that type is designated as "optional" and thus not strictly portable.

A union would do the trick, but the ISO C90 standard does not support unnamed
unions, and using a named union is more pain that it's worth.
*/

static value free_list = 0;

static void clear_free_list(void)
	{
	while (free_list)
		free_memory(Q(0),sizeof(struct value),4);
	}

void beg_value(void)
	{
	emergency = clear_free_list;
	}

void end_value(void)
	{
	clear_free_list();
	end_memory();
	}

/* Increment the reference count. */
value hold(value f)
	{
	if (f) f->N++;
	return f;
	}

/* Decrement the reference count and recycle if it drops to zero. */
void drop(value f)
	{
	if (!f) return;
	if (f->N == 0) die("DROP");
	if (--f->N > 0) return;

	f->N = (unsigned long)free_list;
	free_list = f;
	}

/* Return a value of type T with the given left and right side. */
static value V(type T, value L, value R)
	{
	value f = free_list;
	if (f)
		{
		free_list = (value)f->N;
		if (f->L)
			{
			/* Clear pair. */
			drop(f->L);
			drop(f->R);
			}
		else if (f->R)
			{
			/* Clear atom. */
			f->N = 0;
			f->T(f);
			}
		}
	else
		{
		f = (value)new_memory(sizeof(struct value),4);
		if (!f) return 0;
		}

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

/* Create an atom of type T and data x.  The caller should free x if the result
is 0. */
value D(type T, void *x)
	{
	return V(T,0,x);
	}

/* Evaluate x and apply the result to y. */
value apply(value x, value y)
	{
	value f = (y && (x = eval(x))) ? V(x->T,x,y) : 0;
	if (!f)
		{
		drop(x);
		drop(y);
		}
	return f;
	}

static value type_A(value f)
	{
	return apply(hold(f->L),hold(f->R));
	}

/* Apply x to y. */
value A(value x, value y)
	{
	value f = (x && y) ?  V(type_A,x,y) : 0;
	if (!f)
		{
		drop(x);
		drop(y);
		}
	return f;
	}

/* Return true if x is an atom of type t. */
int is_atom(type t, value x)
	{
	return x && x->T == t && x->L == 0;
	}

void out_of_time(void)
	{
	error_code = "The program ran too long.";
	}

/* Reduce the value to its normal form if possible within current limits. */

unsigned long remain_depth = 50000;
unsigned long remain_steps = 100000000;

value eval(value f)
	{
	if (!f) return 0;
	if (!remain_depth)
		{
		out_of_memory();
		drop(f);
		return 0;
		}
	remain_depth--;
	while (f)
		{
		value g;
		if (remain_steps)
			{
			remain_steps--;
			g = f->T(f);
			}
		else
			{
			out_of_time();
			g = 0;
			}

		if (g == f)
			break;

		drop(f);
		f = g;
		}
	remain_depth++;
	return f;
	}
