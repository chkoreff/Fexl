extern value type_file(value f);
extern value Qfile(FILE *fh, long close);
extern FILE *file_val(value f);
extern value type_base_path(value f);
extern value resolve_file(const char *name);
