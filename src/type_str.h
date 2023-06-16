extern value type_str(value fun, value arg);
extern string get_str(value x);
extern const char *str_data(value x);
extern value Qstr(string x);
extern value Qstr0(const char *data);
extern value type_concat(value fun, value arg);
extern void use_str(void);
