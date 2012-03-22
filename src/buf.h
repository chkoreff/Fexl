struct buf
	{
	char *data;
	long size;
	long len;
	struct buf *prev;
	};

extern void buf_add(struct buf **top, char ch);
extern char *buf_clear(struct buf **top, long *len);
