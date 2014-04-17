extern value type_str(value f);
extern struct str *get_str(value f);
extern value Qstr(struct str *p);
extern value Qstr0(const char *data);
extern value type_concat(value f);
extern value type_string_at(value f);
extern value type_string_cmp(value f);
extern value type_string_len(value f);
extern value type_string_common(value f);
#if 0
extern value type_string_long(value f); /*TODO*/
extern value type_string_double(value f);
#endif
extern value type_string_slice(value f);
extern value type_string_index(value f);
