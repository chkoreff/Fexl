struct sym
	{
	value name;
	int line; /* negative if the name is quoted */
	};

extern value type_sym(value f);
extern struct sym *atom_sym(value f);
extern value Qsym(struct str *name, int line);
extern value Qsym0(const char *name, int line);
extern int sym_eq(struct sym *x, struct sym *y);
extern value app(value f, value g);
extern value lam(value sym, value body);
extern value resolve(value f, value context(value));
