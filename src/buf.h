struct buf
	{
	int pos;
	struct str *str;
	struct buf *next;
	};

extern void buf_start(struct buf *buf);
extern void buf_add(struct buf *buf, char ch);
extern struct str *buf_finish(struct buf *buf);
