typedef struct value *value;

struct type
	{
	value (*step)(value pair);
	value (*apply)(value fun, value arg, value cx);
	void (*clear)(value exp);
	};

struct value
	{
	union
		{
		unsigned long N;
		value next;
		};
	struct type *T;
	value L;
	union
		{
		unsigned long v_u64;
		double v_double; // TODO use
		void *v_ptr;
		value R;
		};
	};

extern value free_list;
extern value new_exp(struct type *type);
extern value hold(value exp);
extern void drop(value exp);
extern void clear_free_list(void);
extern value no_step(value pair);
extern value no_apply(value fun, value arg, value cx);
extern void no_clear(value exp);
extern value (*eval)(value f);
