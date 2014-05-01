extern const char *source_label;
extern unsigned long source_line;
extern value parse_file(const char *name);
extern value resolve_file(const char *name, value context(value));
extern void syntax_error(const char *msg, unsigned long line);
extern void undefined_symbol(const char *name, unsigned long line);
extern value type_die(value f);
