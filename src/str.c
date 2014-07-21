#include <limits.h>
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
	string x;
	if (len > ULONG_MAX - sizeof(struct string)) return 0;
	x = new_memory(sizeof(unsigned long) + 1 + len, 1 + (len >> 3));
	if (!x) return 0;
	x->len = len;
	return x;
	}

string str_new_data(const char *data, unsigned long len)
	{
	string x = str_new(len);
	if (!x) return 0;
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
	free_memory(x, sizeof(unsigned long) + 1 + x->len, 1 + (x->len >> 3));
	}

/* Concatenate x and y. */
string str_concat(string x, string y)
	{
	string z = str_new(x->len + y->len);
	if (!z) return 0;
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

void put_str(string x)
	{
	putd(x->data,x->len);
	}
