struct symbol
	{
	value name;
	unsigned long line;
	value label;
	};

extern value type_sym(value f);
extern value Qsym(string name, unsigned long line, value label);
extern struct symbol *get_sym(value x);
extern string sym_name(value x);
extern unsigned long sym_line(value x);
extern string sym_label(value x);
extern value app(value f, value g);
extern value Qsubst(value p, value e);
extern value lambda(string name, value exp);
extern value op_resolve(value resolve_name(value name), value f);
extern value type_evaluate(value f);
extern value type_resolved(value f);
extern value type_is_resolved(value f);
extern value type_define(value f);
extern value type_subst(value f);
