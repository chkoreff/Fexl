/* A string has a length field, followed by that number of bytes (which may
include NULs), followed by a trailing NUL byte.  The trailing NUL byte makes it
easier to call system functions that expect it.
*/

/* Add a single character to a buffer.  This does very fast buffering, with no
copying needed as the buffer expands dynamically. */
void buf_add(struct buf **top, char ch)
	{
	struct buf *buf = *top;

	if (buf == 0)
		{
		buf = new_memory(sizeof(struct buf));
		*top = buf;
		buf->size = 0;
		buf->len = 0;
		buf->data = 0;
		buf->prev = 0;
		}

	if (buf->len >= buf->size)
		{
		/* Start with block size 32, then double each time up to 1MB limit. */
		long new_size = buf->size == 0 ? 32 : buf->size << 1;
		if (new_size > 1048576) new_size = 1048576;

		char *new_data = new_memory(new_size);

		if (buf->data)
			{
			struct buf *prev = new_memory(sizeof(struct buf));

			prev->size = buf->size;
			prev->len = buf->len;
			prev->data = buf->data;
			prev->prev = buf->prev;

			buf->len = 0;
			buf->prev = prev;
			}

		buf->data = new_data;
		buf->size = new_size;
		}

	buf->data[buf->len++] = ch;
	}

struct atom_string
	{
	void (*free)(struct atom_string *);
	long len;
	char data[];
	};

static void free_string(struct atom_string *x)
	{
	free_memory(x, sizeof(struct atom_string) + x->len + 1);
	}

value type_string(value f)
	{
	return type_error();
	}

/* Allocate a string which can hold len bytes plus trailing NUL. */
static struct atom_string *new_string(long len)
	{
	struct atom_string *x = new_memory(sizeof(struct atom_string) + len + 1);
	x->free = free_string;
	x->len = len;
	return x;
	}

/* Make a string value from the buffer contents. */
value Qstring_buffer(struct buf **buffer)
	{
	struct buf *curr;

	/* Determine total number of bytes held in buffer. */
	curr = *buffer;
	long len = 0;
	while (1)
		{
		if (curr == 0) break;
		len += curr->len;
		curr = curr->prev;
		}

	/* Allocate a string which can hold all the bytes plus trailing NUL. */
	struct atom_string *x = new_string(len);

	/* Copy buffer bytes into the string, freeing chunks along the way. */

	curr = *buffer;
	long offset = len;

	while (1)
		{
		if (curr == 0) break;
		offset -= curr->len;

		memcpy(x->data + offset, curr->data, curr->len);
		free_memory(curr->data, curr->size);

		struct buf *prev = curr->prev;
		free_memory(curr, sizeof(struct buf));
		curr = prev;
		}

	x->data[len] = '\000'; /* add trailing NUL byte */

	return V(type_string, 0, (value)x);
	}

/* Make a string value from the data with the given length. */
value Qstring_data(const char *data, long len)
	{
	if (len < 0) len = 0;

	struct atom_string *x = new_string(len);
	if (data) memcpy(x->data, data, len);
	x->data[len] = '\000'; /* add trailing NUL byte */

	return V(type_string, 0, (value)x);
	}

/* Make a string value from the NUL-terminated chars. */
value Qstring(const char *data)
	{
	return Qstring_data(data, data ? strlen(data) : 0);
	}

const char *string_data(value f)
	{
	struct atom_string *x = (struct atom_string *)f->R;
	return x->data;
	}

long string_len(value f)
	{
	struct atom_string *x = (struct atom_string *)f->R;
	return x->len;
	}

/* Compare strings x and y, returning negative if x < y, zero if x == y, or
positive if x > y. */
int string_cmp(value x, value y)
	{
	if (x == y) return 0;
	long x_len = string_len(x);
	long y_len = string_len(y);

	long min_len = x_len < y_len? x_len : y_len;

	int cmp = memcmp(string_data(x), string_data(y), min_len);

	if (cmp == 0 && x_len != y_len)
		cmp = x_len < y_len ? -1 : 1;

	return cmp;
	}

/* Convert string to long and return true if successful. */
int string_long(const char *beg, long *num)
	{
	char *end;
	*num = strtol(beg, &end, 10);
	return *beg != '\0' && *end == '\0';
	}

/* Convert string to double and return true if successful. */
int string_double(const char *beg, double *num)
	{
	char *end;
	*num = strtod(beg, &end);
	return *beg != '\0' && *end == '\0';
	}

/* Append two strings. */
value type_string_append(value f)
	{
	if (!f->L->L) return 0;

	value x = arg(type_string,&f->L->R);
	value y = arg(type_string,&f->R);

	long xlen = string_len(x);
	long ylen = string_len(y);

	if (xlen == 0)
		return y;
	else if (ylen == 0)
		return x;
	else
		{
		long len = xlen + ylen;

		struct atom_string *z = new_string(len);
		memcpy(z->data, string_data(x), xlen);
		memcpy(z->data + xlen, string_data(y), ylen);
		z->data[len] = '\000'; /* add trailing NUL byte */

		return V(type_string, 0, (value)z);
		}
	}

value resolve_string(const char *name)
	{
	if (strcmp(name,"append") == 0) return Q(type_string_append);
	return 0;
	}

#if 0
LATER more functions
string_cmp
string_slice
string_at
string_eq (maybe just define in fexl)
string_append
string_common
string_len
string_index
#endif
