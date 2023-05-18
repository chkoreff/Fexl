typedef struct value *value;

struct type
	{
	value (*step)(value pair);
	value (*apply)(value fun, value arg, value cx);
	void (*clear)(value exp);
	};

struct ref
	{
	unsigned long pos;
	};

struct lam
	{
	value body;
	};

struct app
	{
	value fun;
	value arg;
	};

struct value
	{
	union
		{
		unsigned long N;
		value next;
		};
	struct type *type;
	union
		{
		struct ref ref;
		struct lam lam;
		struct app app;
		double v_double;
		void *v_ptr;
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
