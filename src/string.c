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

static void free_string(value f)
	{
	free_memory((void *)f->R, (long)f->L + 1);
	}

value type_string(value f)
	{
	return type_C(f);
	}

/* Allocate a string which can hold len bytes plus trailing NUL. */
static value new_string(long len)
	{
	value f = Q(0);
	f->T = (type)free_string;
	f->L = (value)len;
	f->R = (value)new_memory(len + 1);
	return f;
	}

/* Replace with a string using the data with the given length. */
value replace_string_vector(value f, const char *data, long len)
	{
	if (len < 0) len = 0;

	value x = new_string(len);
	char *dest = (char *)x->R;
	if (data) memcpy(dest, data, len);
	dest[len] = '\000'; /* add trailing NUL byte */

	return replace(f, type_string, 0, x);
	}

/* Replace with a string using the NUL-terminated data. */
value replace_string(value f, const char *data)
	{
	return replace_string_vector(f, data, data ? strlen(data) : 0);
	}

/* Replace with a string using the buffer contents. */
value replace_string_buffer(value f, struct buf **buffer)
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
	value x = new_string(len);
	char *dest = (char *)x->R;

	/* Copy buffer bytes into the string, freeing chunks along the way. */

	curr = *buffer;
	long offset = len;

	while (1)
		{
		if (curr == 0) break;
		offset -= curr->len;

		memcpy(dest + offset, curr->data, curr->len);
		free_memory(curr->data, curr->size);

		struct buf *prev = curr->prev;
		free_memory(curr, sizeof(struct buf));
		curr = prev;
		}

	dest[len] = '\000'; /* add trailing NUL byte */

	return replace(f, type_string, 0, x);
	}

value Qstring_vector(const char *data, long len)
	{
	return replace_string_vector(0,data,len);
	}

value Qstring(const char *data)
	{
	return replace_string(0,data);
	}

value Qstring_buffer(struct buf **buffer)
	{
	return replace_string_buffer(0,buffer);
	}

const char *string_data(value f)
	{
	return (const char *)f->R->R;
	}

long string_len(value f)
	{
	return (long)f->R->L;
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

/* Append two strings. */
value type_string_append(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_string,f->L->R);
	value y = arg(type_string,f->R);

	long xlen = string_len(x);
	long ylen = string_len(y);

	value z;
	if (xlen == 0)
		z = y->R;
	else if (ylen == 0)
		z = x->R;
	else
		{
		long len = xlen + ylen;

		z = new_string(len);
		char *dest = (char *)z->R;

		memcpy(dest, string_data(x), xlen);
		memcpy(dest + xlen, string_data(y), ylen);
		dest[len] = '\000'; /* add trailing NUL byte */
		}

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace(f, type_string, 0, z);
	}

/* (string_compare x y lt eq gt) */
value type_string_compare(value f)
	{
	return replace_compare(f,type_string,string_cmp);
	}

value type_is_string(value f)
	{
	return replace_is_type(f, type_string);
	}

static value resolve_string_prefix(const char *name)
	{
	if (strcmp(name,"append") == 0) return Q(type_string_append);
	if (strcmp(name,"compare") == 0) return Q(type_string_compare);
	return 0;
	}

value resolve_string(const char *name)
	{
	if (strncmp(name,"string_",7) == 0) return resolve_string_prefix(name+7);
	if (strcmp(name,"is_string") == 0) return Q(type_is_string);
	return 0;
	}

#if 0
LATER more functions
string_slice
string_at
string_common
string_len
string_index
string_long
string_double
#endif
