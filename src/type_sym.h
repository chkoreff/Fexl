typedef struct symbol *symbol;
struct symbol
	{
	short quoted;
	value name;
	unsigned long line;
	};

extern value type_sym(value f);
extern value Qsym(short quoted, string name, unsigned long line);
extern value app(value f, value g);
extern value lam(value sym, value body);
extern value type_subst(value f);
extern value resolve(value exp, value context(value));
