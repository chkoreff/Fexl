typedef struct value *value;
typedef value (*type)(value);

struct value
	{
	unsigned long N;
	type T;
	value L;
	value R;
	};

extern void end_value(void);
extern value hold(value f);
extern void drop(value f);
extern value Q(type T);
extern value D(type T, void *x);
extern value apply(value x, value y);
extern value A(value x, value y);
extern int is_atom(type t, value x);
extern unsigned long remain_depth;
extern unsigned long remain_steps;
extern value eval(value f);
