extern struct value *stack;
extern void push(struct value *);
extern void pop(void);
extern int arg(struct value *(*T)(struct value *), struct value *f);
