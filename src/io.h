extern void nl(void);
extern void print(const char *);
extern void string_put(struct value *);
extern void quote_string_put(struct value *);
extern struct value *type_nl(struct value *);
extern struct value *type_string_put(struct value *);
extern struct value *type_char_get(struct value *);
extern struct value *type_char_put(struct value *);
