extern value find_item(string name, value cx);
extern struct type type_form;
extern FILE *open_source(const char *name);
extern value load(value label, FILE *fh);
extern value loadf(value label, FILE *fh);
