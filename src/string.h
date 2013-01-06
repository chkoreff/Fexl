struct buf
	{
	char *data;
	long size;
	long len;
	struct buf *prev;
	};

extern void buf_add(struct buf **top, char ch);
extern value type_string(value f);
extern value replace_string(value f, const char *data);
extern value Qstring_vector(const char *data, long len);
extern value Qstring(const char *data);
extern value Qstring_buffer(struct buf **buffer);
extern const char *string_data(value f);
extern long string_len(value f);
extern int string_cmp(value x, value y);
extern value type_string_append(value f);
extern value resolve_string(const char *name);
