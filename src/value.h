typedef struct value *value;
typedef value (*type)(value f);

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

extern value new_value(void);
extern void recycle(value f);
extern value hold(value f);
extern void drop(value f);
extern void clear_free_list(void);
extern void end_value(void);
extern value V(type T, value L, value R);
extern value Q(type T);
extern value A(value x, value y);
extern value keep(value f);
extern unsigned long cur_steps;
extern value eval(value f);
extern value arg(value f);
