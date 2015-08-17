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
extern void reduce(value f, value g);
extern void reduce_Q(value f, type t);
extern void reduce_D(value f, type T, void *data, type destroy);
extern void reduce_A(value f, value x, value y);
extern value eval(value f);
extern value arg(value f);
