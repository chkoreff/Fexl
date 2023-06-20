extern value type_str(value fun, value arg);
extern const char *str_data(value x);
extern value Qstr(string x);
extern value Qstr0(const char *data);
extern value type_concat(value fun, value arg);
