extern void nl(void);
extern void print(const char *);
extern void string_put(value);
extern void quote_string_put(value);
extern int sprintf(char *str, const char *format, ...);

extern value fexl_nl(value);
extern value fexl_string_put(value);
extern value fexl_char_get(value);
extern value fexl_char_put(value);
