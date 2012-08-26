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

If T is zero, the value represents the application of function L to argument R,
which are both non-zero.

If T is non-zero, the value is a normal form.  T is the reduction routine
called during evaluation.  In this case there are three possibilities, as
follows.

If L and R are both zero, the value is a combinator with no arguments.

If L and R are both non-zero, the value is a combinator with arguments, but not
enough to reduce to anything simpler, for example (C x) or (S x y).

If L is zero and R is non-zero, the value is an atomic data type.  R is the
value that holds the data.  R->T is the routine that clears the contents of R
when its reference count drops to zero.  This cleans up the data properly, for
example freeing a string pointer or closing a file handle.
*/

typedef struct value *value;
typedef void (*type)(value);

struct value
	{
	long N;
	type T;
	value L;
	value R;
	};

extern void hold(value);
extern void drop(value);
extern value D(type);
extern value Q(type);
extern value A(value, value);
extern void replace(value, value);
extern void replace_apply(value, value, value);
extern void eval(value);
extern value arg(type, value);
extern int if_type(type, value);
extern void type_error(void);
extern void end_value(void);
