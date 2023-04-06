struct symbol
	{
	value name;
	value pattern;
	unsigned long line;
	};

struct table
	{
	unsigned long count;
	unsigned long len;
	struct symbol vec[];
	};

struct form
	{
	struct table *table;
	value exp;
	value label;
	};

extern value type_form(value f);
extern value Qform(struct form *exp);
extern struct form *form_val(value exp);
extern struct form *form_ref(string name, unsigned long line);
extern struct form *form_join(type t, struct form *fun, struct form *arg);
extern struct form *form_appv(struct form *fun, struct form *arg);
extern struct form *form_app(struct form *fun, struct form *arg);
extern struct form *form_lam(string name, struct form *body);
extern value type_pattern(value f);
extern value type_subst(value f);
extern value type_is_closed(value f);
extern value type_def(value f);
extern value type_value(value f);
extern const char *cur_name;
extern int match(const char *other);
extern value type_resolve(value f);
extern value op_resolve(value f, value define(void));
