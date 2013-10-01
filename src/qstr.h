extern value type_string(value f);
extern value Qstr(struct str *p);
extern value Qstr0(const char *data);
extern struct str *as_str(value f);
extern struct str *get_str(value f);
