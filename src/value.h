typedef struct value *value;
typedef void (*type)(value);

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
extern value V(type T, value L, value R);
extern value Q(type T);
extern value D(type T, type free, value p);
extern value A(value f, value x);
extern value get_D(value f, type t);
extern void replace_V(value f, type T, value L, value R);
extern void replace_D(value f, type T, type free, value p);
extern void replace(value f, value g);
extern void replace_A(value f, value L, value R);

extern unsigned long cur_depth;
extern unsigned long max_depth;

extern unsigned long cur_steps;
extern unsigned long max_steps;

extern void out_of_stack(void);
extern void out_of_time(void);

extern value eval(value f);
