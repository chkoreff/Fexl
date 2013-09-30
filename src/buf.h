struct buf_chunk
	{
	char *data;
	long size;
	long len;
	struct buf_chunk *prev;
	};

struct buf
	{
	struct buf_chunk *top;
	};

extern void buf_start(struct buf *buf);
extern void buf_add(struct buf *buf, char ch);
extern struct str *buf_finish(struct buf *buf);
