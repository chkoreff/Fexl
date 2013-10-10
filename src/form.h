extern value type_form(value f);
extern value apply(value f, value g);
extern value symbol(value quoted, struct str *name, long line);
extern int is_null_name(value x);
extern value first_symbol(value f);
extern value abstract(value sym, value body);
