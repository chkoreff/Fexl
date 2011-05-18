extern void push_var(struct value *var, struct value *def);
extern void pop_var(void);

extern void start_resolve(void);
extern void finish_resolve(void);

extern struct value *resolve(struct value *sym);
