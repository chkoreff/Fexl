typedef struct symbol *symbol;
struct symbol
	{
	short quoted;
	value name;
	unsigned long line;
	};

extern value type_sym(value f);
extern value Qsym(short quoted, value name, unsigned long line);
extern value app(value f, value g);
extern value lam(value sym, value body);
