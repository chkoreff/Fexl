typedef struct value *value;
typedef value (*type)(value);

struct value
	{
	long N;
	type T;
	value L;
	value R;
	};

extern void hold(value f);
extern void drop(value f);
extern value V(type T, value L, value R);
extern value Q(type T);
extern value atom(type T, void *p);
extern void bad_type(void);
extern void *atom_data(value f, type t);
extern value apply(value f, value x);
extern value type_A(value f);
extern value A(value f, value g);
extern value eval(value f);
extern void clear_free_list(void);
extern void end_value(void);
