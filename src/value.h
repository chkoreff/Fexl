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
extern value V(type T, value L, value R);
extern value D(type T, void *p);
extern value Q(type T);
extern void bad_type(void);
extern void *get_data(value f, type t);
extern value apply(value f, value x);
extern value type_A(value f);
extern value A(value f, value x);
extern value eval(value f);
extern value arg(value f);
