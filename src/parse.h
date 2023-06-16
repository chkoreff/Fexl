extern value find_item(string name, value cx); // TODO
extern value type_form(value fun, value arg);
extern FILE *open_source(const char *name);
extern value parse_fexl_fh(value label, FILE *fh);
extern value type_restrict(value fun, value arg);
extern void use_parse(void);
extern value load(value cx, const char *name, FILE *fh); // TODO
