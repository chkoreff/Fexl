#include <value.h>
#include <assert.h>
#include <str.h>
#include <type_double.h>
#include <type_long.h>
#include <type_str.h>

value type_str(value f)
	{
	if (!f->N) str_free(get_str(f));
	return f;
	}

struct str *get_str(value f)
	{
	return (struct str *)get_data(f,type_str);
	}

value Qstr(struct str *p)
	{
	return V(type_str,0,(value)p);
	}

value Qstr0(const char *data)
	{
	return Qstr(str_new_data0(data));
	}

/* (concat x y) is the concatenation of strings x and y. */
value type_concat(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qstr(str_concat(get_str(x),get_str(y)));
	drop(x);
	drop(y);
	return z;
	}

/*TODO test these */

/* (string_at str pos) is the character at pos, or 0 if out of bounds. */
value type_string_at(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qlong(str_at(get_str(x),get_long(y)));
	drop(x);
	drop(y);
	return z;
	}

/* (string_cmp x y) compares x and y and returns <0, 0, or >0. */
value type_string_cmp(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qlong(str_cmp(get_str(x),get_str(y)));
	drop(x);
	drop(y);
	return z;
	}

/* (string_len x) is the length of string x */
value type_string_len(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	value z = Qlong(get_str(x)->len);
	drop(x);
	return z;
	}

/* Compute the length of the longest common prefix of two strings. */
value type_string_common(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value z = Qlong(str_common(get_str(x),get_str(y)));
	drop(x);
	drop(y);
	return z;
	}

/* (string_long x) converts string x to a possible long value. */
#if 0
value type_string_long(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	long num;
	int ok = string_long(get_str(x)->data,&num);
	value result = maybe(ok ? Qlong(num) : 0); /*TODO*/
	drop(x);
	return result;
	}
#endif

/* (string_double x) converts string x to a possible double value. */
#if 0
value type_string_double(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	double num;
	int ok = string_double(get_str(x)->data,&num);
	value result = maybe(ok ? Qdouble(num) : 0); /*TODO*/
	drop(x);
	return result;
	}
#endif

/* (string_slice str pos len)  Consistent with "substr" in Perl. */
value type_string_slice(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	value x = eval(f->L->L->R);
	value y = eval(f->L->R);
	value z = eval(f->R);

	struct str *str = get_str(x);
	long pos = get_long(y);
	long len = get_long(z);

	/* Negative pos: start that far back from end of string. */
	if (pos < 0)
		pos = str->len + pos;

	/* Negative len: leave that many characters off end of string. */
	if (len < 0)
		len = str->len + len - pos;

	/* Negative pos: offset length by that amount and use pos 0. */
	if (pos < 0)
		{
		len += pos;
		pos = 0;
		}

	/* Negative len: use len 0. */
	if (len < 0)
		len = 0;

	/* Compute max allowable length. */
	long max = str->len - pos;
	if (max < 0)
		max = 0;

	/* If it's too long, use max. */
	if (len > max)
		len = max;

	assert(pos >= 0 && len >= 0 && len <= max);
	value result = Qstr(str_new_data(str->data + pos, len));

	drop(x);
	drop(y);
	drop(z);

	return result;
	}

/* (string_index haystack needle offset) Consistent with "index" in Perl. */
value type_string_index(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	value x = eval(f->L->L->R);
	value y = eval(f->L->R);
	value z = eval(f->R);

	struct str *haystack = get_str(x);
	char *xs = haystack->data;
	long xn = haystack->len;

	struct str *needle = get_str(y);
	char *ys = needle->data;
	long yn = needle->len;

	long offset = get_long(z);

	if (offset < 0) offset = 0;
	if (offset > xn) offset = xn;

	value result;

	if (yn == 0)
		/* Always consider null string to be found at adjusted offset. */
		result = Qlong(offset);
	else if (offset + yn > xn)
		/* Avoid unnecessary work if match is impossible based on length. */
		result = Qlong(-1);
	else
		{
		long xi = offset;
		long yi = 0;

		while (1)
			{
			if (yi >= yn)
				{
				result = Qlong(xi - yi);
				break;
				}
			if (xi >= xn)
				{
				result = Qlong(-1);
				break;
				}

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

	drop(x);
	drop(y);
	drop(z);
	return result;
	}
