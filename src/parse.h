extern value parse(FILE *fh, const char *name, long line);
extern value resolve_parse(const char *name);
extern void report_undef(value sym);
