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

char *string_data(struct value *f)
	{
	return (char *)f->R->R;
	}

long string_len(struct value *f)
	{
	return (long)f->R->L;
	}

struct value *type_string(struct value *f) { return f; }

struct value *clear_string(struct value *f)
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
struct value *Qchars(char *data, long len)
	{
	if (data == 0 || len < 0) die("Qchars");

	struct value *atom = create();
	atom->N = 1;
	atom->T = clear_string;
	atom->L = (struct value *)len;
	atom->R = (struct value *)data;

	struct value *value = create();
	value->N = 0;
	value->T = type_string;
	value->L = 0;
	value->R = atom;
	return value;
	}

/* Make a string by copying len bytes of data and adding a NUL byte.  If data
is null then make an uninitialized string which can hold len bytes but don't
copy anything. */
struct value *Qcopy_chars(const char *data, long len)
	{
	if (len < 0) len = 0;

	char *copy = new_memory(len + 1);
	if (data) memcpy(copy, data, len);
	copy[len] = '\000'; /* add trailing NUL byte */

	return Qchars(copy, len);
	}

/* Make a string by copying null-terminated data. */
struct value *Qcopy_string(const char *data)
	{
	if (data == 0) die("Qcopy_string");
	return Qcopy_chars(data, strlen(data));
	}

/* Compare strings x and y, returning negative if x < y, zero if x == y, or
positive if x > y. */
int string_compare(struct value *x, struct value *y)
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
struct value *type_string_compare(struct value *f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L || !f->L->L->L->L->L)
		return f;

	struct value *x = f->L->L->L->L->R;
	struct value *y = f->L->L->L->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_string,y)) return f;

	int cmp = string_compare(x,y);
	if (cmp < 0) return f->L->L->R;
	if (cmp > 0) return f->R;
	return f->L->R;
	}

/* string_slice str pos len */
struct value *type_string_slice(struct value *f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	struct value *x = f->L->L->R;
	struct value *y = f->L->R;
	struct value *z = f->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_long,y)) return f;
	if (!arg(type_long,z)) return f;

	long pos = get_long(y);
	long len = get_long(z);

	long xlen = string_len(x);

	if (pos < 0)
		{
		len += pos;
		pos = 0;
		}

	if (len > xlen - pos)
		len = xlen - pos;

	return Qcopy_chars(string_data(x) + pos, len);
	}

/* string_at str pos - return the character at the position */
struct value *type_string_at(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *x = f->L->R;
	struct value *y = f->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_long,y)) return f;

	long pos = get_long(y);

	long xlen = string_len(x);
	char *xdata = string_data(x);

	long ch = pos >= 0 && pos < xlen ? xdata[pos] : 0;
	return Qlong(ch);
	}

/* Compare strings x and y, returning true if x == y. */
int string_eq(struct value *x, struct value *y)
	{
	long x_len = string_len(x);
	if (x_len != string_len(y)) return 0;
	return memcmp(string_data(x), string_data(y), x_len) == 0;
	}

/* Determine if the value has type string. */
struct value *type_is_string(struct value *f)
	{
	return arg_is_type(type_string,f);
	}

/* Append two strings. */
struct value *type_string_append(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *x = f->L->R;
	struct value *y = f->R;

	if (!arg(type_string,x)) return f;
	if (!arg(type_string,y)) return f;

	long xlen = string_len(x);
	long ylen = string_len(y);

	int len = xlen + ylen;
	struct value *result = Qcopy_chars(0, len);

	char *dest = string_data(result);
	memcpy(dest, string_data(x), xlen);
	memcpy(dest + xlen, string_data(y), ylen);
	return result;
	}

/* Compute the length of the longest common prefix of two strings. */
struct value *type_string_common(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *x = f->L->R;
	struct value *y = f->R;

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
struct value *type_string_len(struct value *f)
	{
	struct value *x = f->R;
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
struct value *type_string_long(struct value *f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	struct value *x = f->L->L->R;
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
struct value *type_string_double(struct value *f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	struct value *x = f->L->L->R;
	if (!arg(type_string,x)) return f;

	double num;
	if (string_double(string_data(x),&num))
		return A(f->R,Qdouble(num));
	return f->L->R;
	}
