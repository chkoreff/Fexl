extern int main_argc;
extern const char **main_argv;
extern void use_file(const char *name);
extern value type_load_extend(value f);
extern void load_main(void);
extern value type_load_argv(value f);
extern value type_load_main(value f);
extern void beg_argv(void);
extern void end_argv(void);
