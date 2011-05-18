extern int sym_eq(struct value *x, struct value *y);
extern void optimize(struct value *value);
extern struct value *abstract(struct value *sym, struct value *body);
