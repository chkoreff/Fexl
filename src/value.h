typedef struct value *value;
typedef value (*type)(value);

struct value
	{
	unsigned long N;
	type T;
	value L;
	value R;
	};

extern void clear_free_list(void);
extern void end_value(void);
extern value hold(value f);
extern void drop(value f);
extern value Q(type T);
extern value D(type T, void *x);
extern value A(value x, value y);
extern value arg(value *f);
extern void *atom(type t, value x);
extern unsigned long remain_depth;
extern unsigned long remain_steps;
extern value eval(value f);
