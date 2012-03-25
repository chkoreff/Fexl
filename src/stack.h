extern value stack;
extern void push(value);
extern void pop(void);
extern void push_list(value *, value);
extern void pop_list(value *);
extern int arg(type, value);
extern value arg_is_type(type, value);
