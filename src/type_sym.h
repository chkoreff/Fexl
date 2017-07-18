extern value type_sym(value f);
extern value Qsym(string name, unsigned long line);
extern const char *sym_name(value x);
extern unsigned long sym_line(value x);
extern value app(value f, value g);
extern value Qsubst(value p, value e);
extern value lambda(const char *name, value exp);
extern value type_subst(value f);
