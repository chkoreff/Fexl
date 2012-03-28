#include <string.h>
#include <stdlib.h> /* strtol, strtod */
#include "die.h"
#include "memory.h"
#include "value.h"
#include "double.h"
#include "long.h"
#include "stack.h"
#include "string.h"

/*TODO wrap a combinator around the buf.c module, which will allow very fast
large-scale buffering */

char *string_data(value f)
	{
	return (char *)f->R->R;
	}

long string_len(value f)
	{
	return (long)f->R->L;
	}

value type_string(value f) { return f; }

value clear_string(value f)
	{
	free_memory(string_data(f), string_len(f) + 1);
	return f;
	}

/*
Make a string from the given data.  The data must by an array of len+1 bytes,
consisting of len bytes of real data, followed by a NUL byte.  The real data
itself may contain anything, including NUL bytes.  We put a NUL byte after the
real data so we can easily call system functions which expect a NUL terminator.
*/
value Qchars(char *data, long len)
	{
	if (data == 0 || len < 0) die("Qchars");

	value atom = create();
	atom->N = 1;
	atom->T = clear_string;
	atom->L = (value)len;
	atom->R = (value)data;

	value f = create();
	f->N = 0;
	f->T = type_string;
	f->L = 0;
	f->R = atom;
	return f;
	}

/* Make a string by copying len bytes of data and adding a NUL byte.  If data
is null then make an uninitialized string which can hold len bytes but don't
copy anything. */
value Qcopy_chars(const char *data, long len)
	{
	if (len < 0) len = 0;

	char *copy = new_memory(len + 1);
	if (data) memcpy(copy, data, len);
	copy[len] = '\000'; /* add trailing NUL byte */

	return Qchars(copy, len);
	}

/* Make a string by copying null-terminated data. */
value Qcopy_string(const char *data)
	{
	if (data == 0) die("Qcopy_string");
	return Qcopy_chars(data, strlen(data));
	}

/* Compare strings x and y, returning negative if x < y, zero if x == y, or
positive if x > y. */
int string_compare(value x, value y)
	{
	long x_len = string_len(x);
	long y_len = string_len(y);

	long min_len = x_len < y_len? x_len : y_len;

	int cmp = memcmp(string_data(x), string_data(y), min_len);

	if (cmp == 0 && x_len != y_len)
		cmp = x_len < y_len ? -1 : 1;

	return cmp;
	}

/* string_compare x y lt eq gt */
value fexl_string_compare(value f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L || !f->L->L->L->L->L)
		return f;

	value x = f->L->L->L->L->R;
	value y = f->L->L->L->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_string,y)) return f;

	int cmp = string_compare(x,y);
	if (cmp < 0) return f->L->L->R;
	if (cmp > 0) return f->R;
	return f->L->R;
	}

/* string_slice str pos len */
value fexl_string_slice(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;
	value y = f->L->R;
	value z = f->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_long,y)) return f;
	if (!arg(type_long,z)) return f;

	long pos = get_long(y);
	long len = get_long(z);

	if (pos < 0)
		{
		len += pos;
		pos = 0;
		}

	long max = string_len(x) - pos;

	if (len > max)
		len = max;

	return Qcopy_chars(string_data(x) + pos, len);
	}

/* string_at str pos - return the character at the position */
value fexl_string_at(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_long,y)) return f;

	long len = string_len(x);
	char *data = string_data(x);

	long pos = get_long(y);

	long ch = pos >= 0 && pos < len ? data[pos] : 0;
	return Qlong(ch);
	}

/* Compare strings x and y, returning true if x == y. */
int string_eq(value x, value y)
	{
	long x_len = string_len(x);
	if (x_len != string_len(y)) return 0;
	return memcmp(string_data(x), string_data(y), x_len) == 0;
	}

/* Determine if the value has type string. */
value fexl_is_string(value f)
	{
	return arg_is_type(type_string,f);
	}

/* Append two strings. */
value fexl_string_append(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_string,y)) return f;

	long xlen = string_len(x);
	long ylen = string_len(y);

	int len = xlen + ylen;
	value result = Qcopy_chars(0, len);

	char *dest = string_data(result);
	memcpy(dest, string_data(x), xlen);
	memcpy(dest + xlen, string_data(y), ylen);
	return result;
	}

/* Compute the length of the longest common prefix of two strings. */
value fexl_string_common(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	value y = f->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_string,y)) return f;

	long xlen = string_len(x);
	long ylen = string_len(y);

	long min_len = xlen < ylen ? xlen : ylen;

	char *xdata = string_data(x);
	char *ydata = string_data(y);

	long len = 0;
	while (len < min_len)
		{
		if (xdata[len] != ydata[len]) break;
		len++;
		}

	return Qlong(len);
	}

/* Return the length of the string. */
value fexl_string_len(value f)
	{
	value x = f->R;
	if (!arg(type_string,x)) return f;
	return Qlong(string_len(x));
	}

/*
Convert string to long.  The value of (string_long string no yes) is:

  no           # if string has incorrect format
  yes long     # if string has correct format

LATER: strtod does not allow numeric separators like ',' and '_'.
*/
int string_long(char *beg, long *num)
	{
	char *end;
	*num = strtol(beg, &end, 10);
	return *beg != '\0' && *end == '\0';
	}

/* string_long x no yes */
value fexl_string_long(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;
	if (!arg(type_string,x)) return f;

	long num;
	if (string_long(string_data(x),&num))
		return A(f->R,Qlong(num));
	return f->L->R;
	}

/*
Convert string to double.  The value of (string_double string no yes) is:

  no           # if string has incorrect format
  yes double   # if string has correct format

LATER: strtod does not allow numeric separators like ',' and '_'.
*/
int string_double(char *beg, double *num)
	{
	char *end;
	*num = strtod(beg, &end);
	return *beg != '\0' && *end == '\0';
	}

/* string_double x no yes */
value fexl_string_double(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;
	if (!arg(type_string,x)) return f;

	double num;
	if (string_double(string_data(x),&num))
		return A(f->R,Qdouble(num));
	return f->L->R;
	}

/*
string_index haystack needle offset
NOTE: I have an extensive test suite verifying compatibility with the Perl
"index" function.
*/
value fexl_string_index(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;
	value y = f->L->R;
	value z = f->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_string,y)) return f;
	if (!arg(type_long,z)) return f;

	char *xs = string_data(x);
	long xn = string_len(x);

	char *ys = string_data(y);
	long yn = string_len(y);

	long zn = get_long(z);
	if (zn < 0) zn = 0;
	if (zn > xn) zn = xn;

	/* Always consider null string to be found at adjusted zn. */
	if (yn == 0) return Qlong(zn);

	/* Avoid unnecessary work if match is impossible based on length. */
	if (zn + yn > xn) return Qlong(-1);

	long xi = zn;
	long yi = 0;

	while (1)
		{
		if (yi >= yn) return Qlong(xi - yi);
		if (xi >= xn) return Qlong(-1);

		if (xs[xi] == ys[yi])
			yi++;
		else
			yi = 0;

		xi++;
		}
	}
