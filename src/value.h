/*
The value is the primary data structure in Fexl, with four fields N, T, L, R.

N is the reference count.  When the count drops to zero or below, the value is
recycled (cleared and freed).

T is the type of the value.

L is the left child, and R is the right child.  If the value is a function
application, these will both be non-zero.  If the value is an atom, the left
child will be zero, and the right child may point to type-specific data of any
kind.
*/
struct value
	{
	int N;
	struct type *T;
	struct value *L;
	struct value *R;
	};

/*
The type structure defines how a value should behave as a high-order function.

The reduce function transforms the value into something simpler if possible,
performing any side effects as it goes.  It returns true if it did something,
or false if there was nothing left to do.

The hold function holds onto the contents of the value when it is copied.  The
drop function drops the contents of the value when it is no longer referenced.

The name is only used for tracing purposes during development.
*/
struct type
	{
	int (*reduce)(struct value *);
	void (*hold)(struct value *);
	void (*drop)(struct value *);
	const char *name;
	};

extern long total_blocks;
extern long total_bytes;
extern long max_bytes;

extern void *new_memory(long num_bytes);
extern void free_memory(void *data, long num_bytes);

extern struct value *new_value(
	struct type *type,
	struct value *left,
	struct value *right);

extern void hold(struct value *value);
extern void drop(struct value *value);

extern int reduce_none(struct value *value);
extern void hold_both(struct value *value);
extern void drop_both(struct value *value);

extern void replace(struct value *value, struct value *other);

extern long total_steps;
extern long max_steps;

extern int max_depth;
extern int total_depth;

extern void evaluate(struct value *value);

extern void exit_bad_type(struct value *value);
extern void evaluate_type(struct value *value, struct type *type);

extern void finish(void);
