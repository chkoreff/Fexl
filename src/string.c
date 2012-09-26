#include <string.h>
#include <stdlib.h> /* strtol, strtod */
#include "buf.h"
#include "die.h"
#include "memory.h"
#include "value.h"
#include "basic.h"
#include "double.h"
#include "long.h"
#include "string.h"

/*LATER wrap a combinator around the buf.c module, which will allow very fast
large-scale buffering */

char *string_data(value f)
	{
	return (char *)f->R->R;
	}

long string_len(value f)
	{
	return (long)f->R->L;
	}

void type_string(value f) { type_error(); }

static void clear_string(value f)
	{
	free_memory(string_data(f), string_len(f) + 1);
	}

/*
Make a string from the given data.  The data must be an array of len+1 bytes,
consisting of len bytes of real data, followed by a NUL byte.  The real data
itself may contain anything, including NUL bytes.  We put a NUL byte after the
real data so we can easily call system functions that expect a NUL terminator.
*/
value Qchars(const char *data, long len)
	{
	if (data == 0 || len < 0) die("Qchars");

	value f = D(type_string);
	f->R->T = clear_string;
	f->R->L = (value)len;
	f->R->R = (value)data;
	return f;
	}

/* Make a string by copying len bytes of data and adding a NUL byte.  If data
is null then make an uninitialized string that can hold len bytes but don't
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

/* Make a string from static null-terminated data, without copying. */
value Qstring(const char *data)
	{
	if (data == 0) die("Qstring");
	value str = Qchars(data, strlen(data));
	str->R->T = 0;  /* Don't clear static string. */
	return str;
	}

/* Concatenate two null-terminated strings. */
value Qstrcat(const char *x, const char *y)
	{
	struct buf *buf = 0;
	buf_add_string(&buf, x);
	buf_add_string(&buf, y);
	long len;
	char *string = buf_clear(&buf,&len);
	return Qchars(string,len);
	}

/* Compare strings x and y, returning negative if x < y, zero if x == y, or
positive if x > y. */
static int string_cmp(value x, value y)
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

/* (string_cmp x y) compares x and y and returns <0, 0, or >0. */
static void reduce2_string_cmp(value f)
	{
	value x = arg(type_string,f->L->R);
	value y = arg(type_string,f->R);

	replace(f, Qlong(string_cmp(x,y)));
	}

void reduce_string_cmp(value f)
	{
	f->T = reduce2_string_cmp;
	}

/* string_slice str pos len */
static void reduce3_string_slice(value f)
	{
	value str = arg(type_string,f->L->L->R);
	long pos = get_long(arg(type_long,f->L->R));
	long len = get_long(arg(type_long,f->R));

	if (pos < 0)
		{
		len += pos;
		pos = 0;
		}

	long max = string_len(str) - pos;
	if (len > max)
		len = max;

	if (pos == 0 && len == max)
		replace(f,str);
	else
		replace(f,Qcopy_chars(string_data(str) + pos, len));
	}

static void reduce2_string_slice(value f)
	{
	f->T = reduce3_string_slice;
	}

void reduce_string_slice(value f)
	{
	f->T = reduce2_string_slice;
	}

/* string_at str pos - return the character at the position */

static void reduce2_string_at(value f)
	{
	value str = arg(type_string,f->L->R);
	long pos = get_long(arg(type_long,f->R));

	long len = string_len(str);
	char *data = string_data(str);

	long ch = pos >= 0 && pos < len ? data[pos] : 0;
	replace(f, Qlong(ch));
	}

void reduce_string_at(value f)
	{
	f->T = reduce2_string_at;
	}

/* Compare strings x and y, returning true if x == y. */
int string_eq(value x, value y)
	{
	if (x == y) return 1;
	long x_len = string_len(x);
	if (x_len != string_len(y)) return 0;
	return memcmp(string_data(x), string_data(y), x_len) == 0;
	}

/* Determine if the value has type string. */
void reduce_is_string(value f)
	{
	replace_boolean(f, if_type(type_string,f->R));
	}

/* Append two strings. */
static void reduce2_string_append(value f)
	{
	value x = arg(type_string,f->L->R);
	value y = arg(type_string,f->R);

	long xlen = string_len(x);
	long ylen = string_len(y);

	if (xlen == 0)
		replace(f,y);
	else if (ylen == 0)
		replace(f,x);
	else
		{
		int len = xlen + ylen;
		value result = Qcopy_chars(0, len);

		char *dest = string_data(result);
		memcpy(dest, string_data(x), xlen);
		memcpy(dest + xlen, string_data(y), ylen);
		replace(f,result);
		}
	}

void reduce_string_append(value f)
	{
	f->T = reduce2_string_append;
	}

/* Compute the length of the longest common prefix of two strings. */
static void reduce2_string_common(value f)
	{
	value x = arg(type_string,f->L->R);
	value y = arg(type_string,f->R);

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

	replace(f, Qlong(len));
	}

void reduce_string_common(value f)
	{
	f->T = reduce2_string_common;
	}

/* Return the length of the string. */
void reduce_string_len(value f)
	{
	value x = arg(type_string,f->R);
	replace(f, Qlong(string_len(x)));
	}

/* Convert string to long and return true if successful. */
int string_long(char *beg, long *num)
	{
	char *end;
	*num = strtol(beg, &end, 10);
	return *beg != '\0' && *end == '\0';
	}

/* (string_long x) converts string x to a possible long value:
  yes long     # if string has correct format
  no           # if string has incorrect format

LATER: strtol does not allow numeric separators like ',' and '_'.
*/
void reduce_string_long(value f)
	{
	value x = arg(type_string,f->R);

	long num;
	if (string_long(string_data(x),&num))
		replace_apply(f, Q(reduce_yes), Qlong(num));
	else
		replace(f, Q(reduce_F));
	}

/* Convert string to double and return true if successful. */
int string_double(char *beg, double *num)
	{
	char *end;
	*num = strtod(beg, &end);
	return *beg != '\0' && *end == '\0';
	}

/* (string_double x yes no) converts string x to a possible double value:
  yes double   # if string has correct format
  no           # if string has incorrect format

LATER: strtod does not allow numeric separators like ',' and '_'.
*/

static void reduce3_string_double(value f)
	{
	value x = arg(type_string,f->L->L->R);

	double num;
	if (string_double(string_data(x),&num))
		replace_apply(f, f->L->R, Qdouble(num));
	else
		replace(f, f->R);
	}

static void reduce2_string_double(value f)
	{
	f->T = reduce3_string_double;
	}

void reduce_string_double(value f)
	{
	f->T = reduce2_string_double;
	}

/*
string_index haystack needle offset
NOTE: I have an extensive test suite verifying compatibility with the Perl
"index" function.
*/
static void reduce3_string_index(value f)
	{
	value x = arg(type_string,f->L->L->R);
	value y = arg(type_string,f->L->R);
	long offset = get_long(arg(type_long,f->R));

	char *xs = string_data(x);
	long xn = string_len(x);

	char *ys = string_data(y);
	long yn = string_len(y);

	if (offset < 0) offset = 0;
	if (offset > xn) offset = xn;

	/* Always consider null string to be found at adjusted offset. */
	if (yn == 0) { replace(f,Qlong(offset)); return; }

	/* Avoid unnecessary work if match is impossible based on length. */
	if (offset + yn > xn) { replace(f,Qlong(-1)); return; }

	long xi = offset;
	long yi = 0;

	while (1)
		{
		if (yi >= yn) { replace(f,Qlong(xi - yi)); return; }
		if (xi >= xn) { replace(f,Qlong(-1)); return; }

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

static void reduce2_string_index(value f)
	{
	f->T = reduce3_string_index;
	}

void reduce_string_index(value f)
	{
	f->T = reduce2_string_index;
	}
