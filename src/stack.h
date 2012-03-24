extern struct value *stack;
extern void push(struct value *);
extern void pop(void);
extern void push_list(struct value **list, struct value *f);
extern void pop_list(struct value **list);
extern int arg(struct value *(*T)(struct value *), struct value *f);
extern struct value *arg_is_type(
	struct value *(*T)(struct value *),
	struct value *f);
