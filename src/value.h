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

The T field is the type, a pointer to a function which reduces a form during
evaluation.  When evaluating a form f, we first evaluate the left side, then
call f->L->T(f).  This yields a new form which is functionally equivalent to f,
and then we continue evaluating f from there.

The L and R fields are the left and right components of the value.  Either one
or both can be zero.

If L == 0, the value is considered to be an atom.  If R != 0, it points to the
additional data for the atom, which includes a function pointer for freeing the
atom when its reference count drops to 0.
*/
typedef struct value *value;
typedef value (*type)(value);

struct value
	{
	long N;
	type T;
	value L;
	value R;
	};

struct atom
	{
	void (*free)(struct atom *);
	long data[];
	};

extern void hold(value);
extern void drop(value);
extern void check(value);
extern value V(type T, value L, value R);
extern value A(value f, value g);
extern value Q(type T);
extern value eval(value *pos);
extern value type_error(void);
extern value arg(type T, value *x);
extern void end_value(void);
