typedef struct buffer *buffer;

struct buffer
	{
	size_t len;
	size_t size;
	char *data;
	};

extern void buf_add(buffer buf, char ch);
extern void buf_addn(buffer buf, const char *str, size_t len);
extern void buf_put(buffer buf, string str);
extern void buf_discard(buffer buf);
extern string buf_clear(buffer buf);
