typedef struct string *string;

struct string
	{
	unsigned long len;
	char data[];
	};

extern string str_new(unsigned long len);
extern string str_new_data(const char *data, unsigned long len);
extern string str_new_data0(const char *data);
extern string str_copy(string x);
extern void str_free(string x);
extern string str_concat(string x, string y);
extern int str_cmp(string x, string y);
extern int str_eq(string x, string y);
extern string str_slice(string x, unsigned long pos, unsigned long len);
extern unsigned long str_search(string x, string y, unsigned long offset);
extern string dirname(string path);
extern string basename(string path);
extern unsigned long length_common(string x, string y);
extern unsigned char char_width(unsigned char ch);
