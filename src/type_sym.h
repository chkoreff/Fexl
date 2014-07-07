struct sym
	{
	int quoted;
	value name;
	unsigned long line;
	};

extern void type_sym(value f);
extern value Qsym(int quoted, string name, unsigned long line);
extern struct sym *get_sym(value f);
extern value app(value f, value g);
extern value lam(value sym, value body);
extern value resolve(value f, value context(value));
