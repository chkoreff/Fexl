typedef struct buffer buffer;

struct buffer
	{
	unsigned long pos;
	string str;
	buffer *next;
	};

extern void buf_start(buffer *buf);
extern void buf_add(buffer *buf, char ch);
extern string buf_finish(buffer *buf);
