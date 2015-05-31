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
extern value D(type T, void *data, type destroy);
extern void *data(value f);
extern value A(value x, value y);
extern void replace(value f, value g);
extern void replace_Q(value f, type t);
extern void replace_D(value f, type T, void *data, type destroy);
extern void replace_A(value f, value x, value y);
extern value eval(value f);
