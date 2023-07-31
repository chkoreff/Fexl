typedef struct value *value;
typedef value (*type)(value fun, value f);

struct value
	{
	union
		{
		unsigned long N;
		value next;
		};
	union
		{
		type T;
		void (*clear)(value);
		};
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
extern void end_value(void);
extern value keep(value fun, value f);
extern value V(type T, value L, value R);
extern value Qdouble(type T, value L, double v_double);
extern value Q(type T);
extern value A(value x, value y);
extern unsigned long cur_steps;
extern value eval(value f);
extern value arg(value f);
