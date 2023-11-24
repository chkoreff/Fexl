typedef struct buffer *buffer;

struct chunk
	{
	unsigned long pos;
	string str;
	struct chunk *next;
	};

struct buffer
	{
	struct chunk *top;
	};

extern void buf_add(buffer buf, char ch);
extern void buf_addn(buffer buf, const char *str, unsigned long len);
extern void buf_put(buffer buf, string str);
extern void buf_discard(buffer buf);
extern unsigned long buf_length(buffer buf);
extern string buf_clear(buffer buf);
