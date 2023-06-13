typedef struct value *value;
typedef value (*type)(value);

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
extern value V(type T, value L, value R);
extern value Qdouble(type T, value L, double v_double);
extern value Q(type T);
extern value AV(value x, value y);
extern value type_A(value f);
extern value A(value x, value y);
extern value (*eval)(value f);
extern value arg(value f);
