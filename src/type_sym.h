struct symbol
	{
	value name;
	unsigned long line;
	value pattern;
	};

extern value type_sym(value f);
extern value Qsubst;
extern value Qevaluate;
extern value type_form(value f);
extern value Qform(value label, value exp);
extern value form_val(value exp);
extern value form_ref(value name, unsigned long line);
extern value form_join(type t, value fun, value arg);
extern value form_appv(value fun, value arg);
extern value form_app(value fun, value arg);
extern value form_lam(value name, value body);
extern value type_subst(value f);
extern value type_evaluate(value f);
extern value type_resolve(value f);
