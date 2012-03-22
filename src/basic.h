extern struct value *type_C(struct value *);
extern struct value *type_S(struct value *);
extern struct value *type_I(struct value *);
extern struct value *type_R(struct value *);
extern struct value *type_L(struct value *);
extern struct value *type_Y(struct value *);
extern struct value *type_query(struct value *);
extern struct value *type_lambda(struct value *);

extern struct value *C;
extern struct value *S;
extern struct value *I;
extern struct value *R;
extern struct value *L;
extern struct value *Y;
extern struct value *F;
extern struct value *query;
extern struct value *lambda(struct value *x, struct value *f);

extern void beg_basic(void);
extern void end_basic(void);
