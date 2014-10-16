extern void syntax_error(const char *code, unsigned long line);
extern void undefined_symbol(const char *name, unsigned long line);
extern value eval_file(const char *name);
extern value type_eval_file(value f);
extern value type_eval_str(value f);
