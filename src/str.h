struct str
	{
	int len;
	char data[];
	};

extern struct str *str_new(int len);
extern struct str *str_new_data(const char *data, int len);
extern struct str *str_new_data0(const char *data);
extern void str_free(struct str *x);
extern char str_at(struct str *str, int pos);
extern struct str *str_concat(struct str *x, struct str *y);
extern int str_cmp(struct str *x, struct str *y);
extern int str_eq(struct str *x, struct str *y);
extern int str_common(struct str *x_str, struct str *y_str);
