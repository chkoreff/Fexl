extern value type_form(value f);
extern value type_name(value f);
extern value Qname(struct str *p);
extern value apply(value f, value g);
extern value symbol(type t, struct str *name, long line);
extern int is_null_name(value x);
extern value first_symbol(value f);
extern value fuse(value f, value g);
extern value abstract(value sym, value body);
