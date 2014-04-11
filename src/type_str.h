extern value type_str(value f, value g);
extern struct str *atom_str(value f);
extern value Qstr(struct str *p);
extern value Qstr0(const char *data);
extern value type_concat(value f, value g);
