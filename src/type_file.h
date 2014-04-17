extern value type_file(value f);
extern FILE *get_file(value f);
extern value Qfile(FILE *fh);
extern value type_putchar(value f);
extern value type_getchar(value f);
extern value type_fputc(value f);
extern value type_fgetc(value f);
extern value type_fwrite(value f);
#if 0
extern value type_fopen(value f); /*TODO*/
#endif
extern value type_readlink(value f);
extern void print(value f);
extern void nl(void);
extern value type_nl(value f);
extern value type_print(value f);
extern value type_say(value f);
