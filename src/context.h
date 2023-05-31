extern value cx_cur;
extern void define(const char *key, value val);
extern void define_op(const char *name, value op(void));
extern void beg_context(void);
extern void end_context(void);
