/* A string has a length field, followed by that number of bytes (which may
include NULs), followed by a trailing NUL byte.  The trailing NUL byte makes it
easier to call system functions that expect it.
*/
struct str
	{
	long len;
	char data[];
	};

extern struct str *str_new(long len);
extern struct str *str_new_data(const char *data, long len);
extern struct str *str_new_data0(const char *data);
extern void str_free(struct str *x);
extern long str_at(struct str *str, long pos);
extern struct str *str_concat(struct str *x, struct str *y);
extern int str_cmp(struct str *x, struct str *y);
extern long str_common(struct str *x_str, struct str *y_str);
