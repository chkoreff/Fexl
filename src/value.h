typedef struct value *value;
typedef value (*type)(value);

struct value
	{
	unsigned long N;
	type T;
	value L;
	value R;
	};

extern value hold(value f);
extern void drop(value f);
extern void end_value(void);
extern value V(type T, value L, value R);
extern value Q(type T);
extern value D(type T, void *data);
extern void *data(value f);
extern value type_A(value f);
extern value A(value x, value y);
extern value type_J(value f);
extern value reduce(value f, value g);
extern value reduce_Q(value f, type T);
extern value reduce_D(value f, type T, void *data);
extern value reduce_A(value f, value x, value y);
extern value eval(value f);
extern value arg(value f);
