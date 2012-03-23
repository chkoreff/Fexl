struct buf
	{
	char *data;
	long size;
	long len;
	struct buf *prev;
	};

extern void buf_add(struct buf **top, char ch);
extern void buf_add_string(struct buf **top, char *str);
extern char *buf_clear(struct buf **top, long *len);
