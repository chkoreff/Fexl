extern FILE *source_fh;
extern const char *source_name;
extern long source_line;

extern value parse(FILE *fh, const char *name, long line);
extern void report_undef(value sym);
