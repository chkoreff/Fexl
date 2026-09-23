extern value type_str(value f);
extern value Qstr(string x);
extern value Qstr0(const char *data);
extern const char *str_data(value x);
extern value op_str(value f, string op(string));
extern value op_str2(value f, string op(string,string));
extern value op_str3(value f, string op(string,string,string));
extern value op_argv(value f, value op(const char *const *argv));
extern value type_load_str(value f);
