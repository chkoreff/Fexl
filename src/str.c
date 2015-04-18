#include <memory.h>
#include <output.h>
#include <str.h>
#include <string.h>

/* A string has a length field, followed by that number of bytes (which may
include NULs), followed by a trailing NUL byte.  The trailing NUL byte makes it
easier to call system functions that expect it.
*/

/* Create a string capable of holding len bytes plus trailing NUL. */
string str_new(unsigned long len)
	{
	string x = new_memory(sizeof(unsigned long) + 1 + len);
	x->len = len;
	return x;
	}

string str_new_data(const char *data, unsigned long len)
	{
	string x = str_new(len);
	memcpy(x->data, data, len);
	x->data[len] = '\000';
	return x;
	}

string str_new_data0(const char *data)
	{
	return str_new_data(data, strlen(data));
	}

void str_free(string x)
	{
	free_memory(x, sizeof(unsigned long) + 1 + x->len);
	}

/* Concatenate x and y. */
string str_concat(string x, string y)
	{
	string z = str_new(x->len + y->len);
	memcpy(z->data, x->data, x->len);
	memcpy(z->data + x->len, y->data, y->len);
	z->data[z->len] = '\000'; /* add trailing NUL byte */
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

void put_str(string x)
	{
	putd(x->data,x->len);
	}
