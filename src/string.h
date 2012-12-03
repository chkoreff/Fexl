struct buf
	{
	char *data;
	long size;
	long len;
	struct buf *prev;
	};

extern void buf_add(struct buf **top, char ch);
extern value type_string(value f);
extern value Qstring_buffer(struct buf **buffer);
extern value Qstring_data(const char *data, long len);
extern value Qstring(const char *data);
extern const char *string_data(value f);
extern long string_len(value f);
extern int string_cmp(value x, value y);
extern int string_long(const char *beg, long *num);
extern int string_double(const char *beg, double *num);
extern int sprintf(char *str, const char *format, ...);
extern value type_string_append(value f);
extern value resolve_string(const char *name);
