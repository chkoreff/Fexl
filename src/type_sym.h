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

extern void form_free(struct form *form);
extern struct form *form_val(value exp);
extern struct form *form_ref(string name, unsigned long line);
extern struct form *form_lam(const char *name, struct form *body);
extern struct form *form_app(struct form *form_1, struct form *form_2);
extern struct form *form_cons(struct form *head, struct form *tail);
extern struct form *form_tuple(struct form *form);
extern struct form *form_eval(struct form *def, struct form *exp);
extern struct form *form_quo(struct form *form);
extern value type_subst(value f);
extern value type_form(value f);
extern value type_evaluate(value f);
extern value type_resolve(value f);
