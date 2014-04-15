#include <die.h>
#include <memory.h>

#include <value.h>

/*
In the value structure, the reference count N is a long int, which guarantees
that it can also be used as a pointer to link unused values on the free list,
without having to use a cumbersome union type.

The standard says:

  long ints are large enough to hold pointers in -n32 and -o32 mode.  Both are
  32 bits wide.

  long ints are large enough to hold pointers in -64 mode.  Both are 64 bits
  wide.

(C Language Reference Manual, document 007-0701-150, Appendix A, Section F.3.7
"Arrays and Pointers")

Assuming there are no other modes, I conclude that long ints are large enough
to hold pointers, period.

As further evidence, the section titled "Integer and Floating Point Types" has
a table of Storage Class Sizes that lists the size in bits for the various
types in all three modes -o32, -n32, and -64.  In all modes the size of a long
equals the size of a pointer.

An integer overflow in the reference count is impossible because of memory
limits.  On a machine with an N-bit word, an overflow is when we increment the
reference count after it has already reached its maximum value of 2^(N-1)-1.
This can only happen if we have created 2^(N-1) value records all pointing to a
single place.  Because each value record is 4 words long, that means we have
allocated 4*2^(N-1) words, which is 2^(N+1) words.  But that is impossible on
an N-bit machine, which can only address 2^N words.  Therefore an integer
overflow is impossible.

The T field is the type, a pointer to a C routine which determines what the
value does with a new argument passed to it.  The routine also serves to clear
an atom whose reference count drops to 0.

The L and R fields are the left and right components of the value.
If L == 0, the value is an atom, and R is the atom data, if any.
If L != 0, then R != 0, and the value represents the application of L to R.
*/

static value free_list = 0;

/* Put the value on the free list. */
static void recycle(value f)
	{
	f->N = (long)free_list;
	free_list = f;
	}

/* Increment the reference count. */
void hold(value f)
	{
	f->N++;
	}

/*
Decrement the reference count and put the value on the free list if it drops to
zero.  We are very strict about this.  It must be exactly zero, not negative.
A negative reference count indicates an imbalance in hold/drop calls, which is
sloppy programming, and in that case we want to report a memory leak error when
the program ends.
*/
void drop(value f)
	{
	if (--f->N == 0) recycle(f);
	}

/* Return a value of type T with the given left and right side. */
value V(type T, value L, value R)
	{
	if (L)
		{
		hold(L);
		hold(R);
		}

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
			f->T(f,0);
			}
		}
	else
		f = (value)new_memory(sizeof (struct value));

	f->N = 0;
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

/* Create a new atom with the given type and data pointer. */
value atom(type T, void *p)
	{
	return V(T,0,(value)p);
	}

void bad_type(void)
	{
	die("You used a data type incorrectly.");
	}

/* Return the data pointer of an atom of type t. */
void *atom_data(value f, type t)
	{
	if (f->L != 0 || f->T != t) bad_type();
	return f->R;
	}

static value type_A(value f, value g)
	{
	return combine(f->L->T(f->L,f->R), g);
	}

/* Apply f to g lazily. */
value A(value f, value g)
	{
	return V(type_A,f,g);
	}

/* Apply f to g lazily, but with the result the same type as f. */
value collect(value f, value g)
	{
	return V(f->T,f,g);
	}

/* Apply f to g eagerly. */
value combine(value f, value g)
	{
	hold(f);
	hold(g);
	value h = f->T(f,g);
	hold(h);
	drop(f);
	drop(g);
	h->N--;
	return h;
	}

/* Evaluate the value, returning its normal form if possible within any limits
on space and time. */
value eval(value f)
	{
	hold(f);
	while (f->T == type_A)
		{
		value g = f->L->T(f->L,f->R);
		hold(g);
		drop(f);
		f = g;
		}
	return f;
	}

void clear_free_list(void)
	{
	while (free_list)
		free_memory(Q(0), sizeof(struct value));
	}

void end_value(void)
	{
	clear_free_list();
	end_memory();
	}
