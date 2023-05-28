extern struct type type_str;
extern string get_str(value x);
extern const char *str_data(value x);
extern value Qstr(string x);
extern value Qstr0(const char *data);
extern void def_type_str(void);
