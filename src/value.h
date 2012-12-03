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
