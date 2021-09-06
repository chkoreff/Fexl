#include <memory.h>
#include <str.h>
#include <string.h> /* memcpy strlen */

/* A string has a length field, followed by that number of bytes (which may
include NULs), followed by a trailing NUL byte.  The trailing NUL byte makes it
easier to call system functions that expect it.
*/

/* Return the number of bytes needed to store a string of the given length. */
static unsigned long str_size(unsigned long len)
	{
	return sizeof(struct string) + len + 1;
	}

/* Create a string capable of holding len bytes plus trailing NUL. */
string str_new(unsigned long len)
	{
	string x = new_memory(str_size(len));
	x->len = len;
	x->data[len] = '\000'; /* Set trailing NUL byte. */
	return x;
	}

string str_new_data(const char *data, unsigned long len)
	{
	string x = str_new(len);
	memcpy(x->data, data, len);
	return x;
	}

string str_new_data0(const char *data)
	{
	return str_new_data(data, strlen(data));
	}

void str_free(string x)
	{
	free_memory(x, str_size(x->len));
	}

/* Concatenate x and y. */
string str_concat(string x, string y)
	{
	string z = str_new(x->len + y->len);
	memcpy(z->data, x->data, x->len);
	memcpy(z->data + x->len, y->data, y->len);
	return z;
	}

/* Compare strings x and y, returning negative if x < y, zero if x == y, or
positive if x > y. */
int str_cmp(string x, string y)
	{
	if (x == y) return 0;
	{
	unsigned long min_len = x->len < y->len ? x->len : y->len;
	int cmp = memcmp(x->data, y->data, min_len);

	if (cmp == 0 && x->len != y->len)
		cmp = x->len < y->len ? -1 : 1;

	return cmp;
	}
	}

/* Return true if the strings are equal. */
int str_eq(string x, string y)
	{
	if (x == y) return 1;
	if (x->len != y->len) return 0;
	return memcmp(x->data, y->data, x->len) == 0;
	}

/* Return the len bytes of x starting at pos, clipping if necessary to stay
within the bounds of x. */
string str_slice(string x, unsigned long pos, unsigned long len)
	{
	if (pos > x->len)
		pos = x->len;

	if (len > x->len - pos)
		len = x->len - pos;

	return str_new_data(x->data + pos,len);
	}

/* Search x for the first occurrence of y, starting at offset.  Return the
position within x where y was found.  If the position returned is past the end
of x, then y was not found. */
unsigned long str_search(string x, string y, unsigned long offset)
	{
	unsigned long xn = x->len;
	unsigned long yn = y->len;

	/* Avoid unnecessary work if a match is impossible based on lengths. */
	if (xn < yn || offset > xn - yn) return xn;

	{
	char *xs = x->data;
	char *ys = y->data;
	unsigned long xi = offset;
	unsigned long yi = 0;
	while (1)
		{
		if (yi >= yn) return xi - yi; /* found */
		if (xi >= xn) return xn; /* not found */

		if (xs[xi] == ys[yi])
			yi++;
		else
			{
			xi -= yi;
			yi = 0;
			}

		xi++;
		}
	}
	}

/* Strip the last component from a file name.  See dirname(1). */
string dirname(string path)
	{
	char *buf = path->data;
	unsigned long len = path->len;

	while (len > 0 && buf[len-1] == '/') len--;
	while (len > 0 && buf[len-1] != '/') len--;
	while (len > 1 && buf[len-1] == '/') len--;

	if (len == 0)
		return str_new_data(".",1);
	else
		return str_new_data(buf,len);
	}

/* Strip the directory from a file name.  See basename(1). */
string basename(string path)
	{
	char *buf = path->data;
	unsigned long len = path->len;

	while (len > 1 && buf[len-1] == '/') len--;

	if (len > 0 && buf[len-1] == '/')
		return str_new_data("/",1);
	else
		{
		unsigned long pos = len;
		while (pos > 0 && buf[pos-1] != '/') pos--;
		return str_new_data(buf+pos,len-pos);
		}
	}

/* Return the number of starting bytes the strings have in common. */
unsigned long length_common(string x, string y)
	{
	unsigned long min_len = x->len < y->len ? x->len : y->len;

	const char *xd = x->data;
	const char *yd = y->data;

	unsigned long len = 0;
	while (len < min_len)
		{
		if (xd[len] != yd[len]) break;
		len++;
		}
	return len;
	}
