typedef struct value *value;
typedef struct type *type;

struct type
	{
	value (*step)(value f);
	value (*apply)(value f, value x);
	void (*clear)(value f);
	};

struct value
	{
	union
		{
		unsigned long N;
		value next;
		};
	type T;
	value L;
	union
		{
		value R;
		void *v_ptr;
		double v_double;
		};
	};

extern value hold(value f);
extern void drop(value f);
extern void clear_free_list(void);
extern value V(type T, value L, value R);
extern value Q(type T, void *p);
extern value Qdouble(type T, double x);
extern value step_A(value f);
extern void clear_A(value f);
extern struct type type_A;
extern value A(value x, value y);
extern void no_clear(value f);
extern struct type type_Z;
extern value eval(value f);
