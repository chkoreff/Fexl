extern int argc;
extern char **argv;
extern char **envp;

extern FILE *source_fh;
extern const char *source_label;
extern int source_line;

extern value type_argv(value f);
extern value type_exit(value f);
extern value type_setrlimit(value f);
