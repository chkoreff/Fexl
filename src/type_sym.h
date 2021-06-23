struct symbol
	{
	struct symbol *next;
	string name;
	unsigned long line;
	value pattern;
	};

struct form
	{
	struct symbol *sym;
	value exp;
	value label;
	};

extern value Qsubst;
extern value Qevaluate;
extern value type_form(value f);
extern value Qform(struct form *exp);
extern struct form *form_val(value exp);
extern struct form *form_ref(string name, unsigned long line);
extern struct form *form_join(type t, struct form *fun, struct form *arg);
extern struct form *form_appv(struct form *fun, struct form *arg);
extern struct form *form_app(struct form *fun, struct form *arg);
extern struct form *form_lam(const char *name, struct form *body);
extern value type_subst(value f);
extern value type_evaluate(value f);
extern value type_resolve(value f);
