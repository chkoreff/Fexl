#include <stdint.h>

#include <str.h>
#include <types.h>

#include <bn.h>
#include <buffer.h>
#include <die.h>
#include <memory.h>
#include <string.h> // strlen

// These routines manipulate signed integers of arbitrary size.
//
// Numbers are represented in base B = 2^32, as a sequence of u32 words with a
// maximum length of (B-1).  That allows a number as high as B^(B-1)-1, which
// would require 2^34 bytes to store (over 17 GB).
//
// Most of the vector routines run in constant time for given operand sizes,
// with the exceptions of vec_nsd, vec_cmp, and vec_div.

// Return the number of significant digits in x.
static u32 vec_nsd(u32 n, const u32 *x)
	{
	while (n > 0 && x[n-1] == 0)
		n--;
	return n;
	}

// Fill x with the constant value v.
static void vec_fill(u32 n, u32 *x, u32 v)
	{
	u32 i;
	for (i = 0; i < n; i++)
		x[i] = v;
	}

// Fill x with 0.
static void vec_zero(u32 n, u32 *x)
	{
	vec_fill(n,x,0);
	}

// Copy x to y.
static void vec_copy(u32 n, const u32 *x, u32 *y)
	{
	u32 i;
	for (i = 0; i < n; i++)
		y[i] = x[i];
	}

// Compare x and y, returning the sign s where:
//   (s =  0 and x = y) or
//   (s =  1 and x > y) or
//   (s = -1 and x < y)
static int vec_cmp(u32 n, const u32 *x, const u32 *y)
	{
	while (n > 0)
		{
		n--;
		if (x[n] < y[n])
			return -1;
		else if (x[n] > y[n])
			return 1;
		}
	return 0;
	}

static int nat_cmp(const struct bn *x, const struct bn *y)
	{
	if (x->nsd < y->nsd)
		return -1;
	else if (x->nsd > y->nsd)
		return 1;
	else
		return vec_cmp(x->nsd,x->vec,y->vec);
	}

// Add u32 digit to vector and return carry.
static u32 vec_add_u32(u32 n, const u32 *x, u32 carry, u32 *z)
	{
	u32 i;
	for (i = 0; i < n; i++)
		{
		u64 sum = carry + (u64)x[i];
		z[i] = sum;
		carry = (sum >> 32);
		}
	return carry;
	}

// Add i32 digit to vector and return carry.
static i32 vec_add_i32(u32 n, const u32 *x, i32 carry, u32 *z)
	{
	u32 i;
	for (i = 0; i < n; i++)
		{
		i64 sum = carry + (i64)x[i];
		z[i] = sum;
		carry = (sum >> 32);
		}
	return carry;
	}

// Add vectors of the same size and return carry.
static u32 vec_add_same(u32 n, const u32 *x, const u32 *y, u32 *z)
	{
	u32 carry = 0;
	u32 i;
	for (i = 0; i < n; i++)
		{
		u64 sum = carry + (u64)x[i] + (u64)y[i];
		z[i] = sum;
		carry = (sum >> 32);
		}
	return carry;
	}

// Subtract vectors of the same size and return carry.
static i32 vec_sub_same(u32 n, const u32 *x, const u32 *y, u32 *z)
	{
	i32 carry = 0;
	u32 i;
	for (i = 0; i < n; i++)
		{
		i64 sum = carry + (i64)x[i] - (i64)y[i];
		z[i] = sum;
		carry = (sum >> 32);
		}
	return carry;
	}

// Add vectors where xn >= yn.
static void vec_add(u32 xn, const u32 *x, u32 yn, const u32 *y, u32 *z)
	{
	u32 carry = vec_add_same(yn,x,y,z);
	carry = vec_add_u32(xn-yn,x+yn,carry,z+yn);
	z[xn] = carry;
	}

// Subtract vectors where xn >= yn.
static void vec_sub(u32 xn, const u32 *x, u32 yn, const u32 *y, u32 *z)
	{
	i32 carry = vec_sub_same(yn,x,y,z);
	vec_add_i32(xn-yn,x+yn,carry,z+yn);
	}

// Set z = x*y.
static void vec_mul(u32 xn, const u32 *x, u32 yn, const u32 *y, u32 *z)
	{
	u32 i,j;
	vec_zero(xn+yn,z);
	for (i = 0; i < yn; i++)
		{
		u32 *p = z+i;
		u32 digit = y[i];
		u32 carry = 0;
		for (j = 0; j < xn; j++)
			{
			u64 sum = carry + (u64)p[j] + ((u64)x[j] * (u64)digit);
			p[j] = sum;
			carry = (sum >> 32);
			}
		p[xn] = carry;
		}
	}

// Set x = x*scale + digit.
static void vec_append(u32 n, u32 *x, u32 scale, u32 digit)
	{
	u32 i;
	for (i = 0; i < n; i++)
		{
		u64 sum = (u64)x[i] * (u64)scale + (u64)digit;
		x[i] = sum;
		digit = (sum >> 32);
		}
	x[n] = digit;
	}

// Convert a string of decimal digits to a number.
static void vec_from_dec(const char *s, u32 n, u32 *x)
	{
	const u8 max_chunk = 9; // Must be in range 1-9.
	u64 pos = 0;

	vec_zero(n,x);
	while (1)
		{
		u32 digit = 0;
		u32 scale = 1;
		u32 i;

		for (i = 0; i < max_chunk; i++)
			{
			char c = s[pos];
			u8 d;

			if (c >= '0' && c <= '9')
				d = (c-'0');
			else
				break;

			digit = 10*digit + d;
			scale *= 10;
			pos++;
			}

		if (scale == 1)
			break;

		vec_append(n-1,x,scale,digit); // Leave room for carry.
		}
	}

// Set r = r - y*qd, where r >= y*qd.
static void vec_sub_mul(u32 *r, u32 yn, const u32 *y, u32 qd)
	{
	u32 c_prod = 0; // carry for product
	i32 c_diff = 0; // carry for difference
	u32 i;

	for (i = 0; i < yn; i++)
		{
		u64 prod = c_prod + (u64)qd * (u64)y[i];
		i64 diff = c_diff + (i64)r[i] - (u32)prod;

		r[i] = diff;
		c_prod = (prod >> 32);
		c_diff = (diff >> 32);
		}

	// If final carry is nonzero then r[yn] is guaranteed to exist.
	{
	i64 carry = (i64)c_diff - (i64)c_prod;
	if (carry)
		r[yn] += carry;
	}
	}

// Divide x by y, and return quotient q and remainder r where:
//   x = q*y + r  and  (y=0 or 0 <= r < y)
//
// I do not optimize the case where y=1 because I want constant time operation
// as far as possible and you probably don't divide by 1 too often anyway.
//
// On each round I calculate a guess with digits {qd qn} where:
//   (qd = 0 and x < y) or
//   (qd > 0 and x >= (y * qd * base^qn))
static void vec_div
	(
	u32 xn, const u32 *x,
	u32 yn, const u32 *y,
	u32 *q, u32 *r
	)
	{
	vec_copy(xn,x,r);
	if (q)
		vec_zero(xn,q);
	if (yn == 0) return;
	{
	u64 y0 = y[yn-1];
	while (xn >= yn)
		{
		u64 x0 = r[xn-1];
		u64 yd = (yn == 1) ? y0 : 1+y0;
		u32 qd, qn;

		if (x0 >= yd)
			{
			qd = x0 / yd;
			qn = xn - yn;
			}
		else if (xn == yn)
			{
			if (vec_cmp(xn,r,y) < 0) break;
			qd = 1;
			qn = 0;
			}
		else
			{
			x0 = (x0 << 32) + r[xn-2];
			qd = x0 / yd;
			qn = xn - yn - 1;
			}

		if (q)
			vec_add_u32(xn-qn,q+qn,qd,q+qn);
		vec_sub_mul(r+qn,yn,y,qd);
		xn = vec_nsd(xn,r);
		}
	}
	}

// Divide x by y and return remainder without computing quotient.
static void vec_mod(u32 xn, const u32 *x, u32 yn, const u32 *y, u32 *r)
	{
	vec_div(xn,x,yn,y,0,r);
	}

// Return the number of bytes needed to store a bn of the given length.
static u64 bn_size(u64 n)
	{
	return sizeof(struct bn) + sizeof(u32[n]);
	}

// Create an uninitialized bn of length n, where (0 <= n <= UINT32_MAX).
struct bn *bn_new(u64 n)
	{
	if (n > UINT32_MAX)
		{
		die("bn_size out of bounds");
		return 0;
		}
	else
		{
		struct bn *x = new_memory(bn_size(n));
		x->len = n;
		return x;
		}
	}

void bn_free(struct bn *x)
	{
	free_memory(x,bn_size(x->len));
	}

int bn_eq0(const struct bn *x)
	{
	return x->nsd == 0;
	}

int bn_is_neg(const struct bn *x)
	{
	return x->sign;
	}

struct bn *bn_neg(const struct bn *x)
	{
	struct bn *y = bn_new(x->nsd);
	y->sign = x->nsd ? (1 - x->sign) : 0;
	y->nsd = x->nsd;
	vec_copy(x->nsd,x->vec,y->vec);
	return y;
	}

int bn_cmp(const struct bn *x, const struct bn *y)
	{
	if (x->sign)
		{
		if (y->sign)
			return nat_cmp(y,x);
		else
			return -1;
		}
	else
		{
		if (y->sign)
			return 1;
		else
			return nat_cmp(x,y);
		}
	}

static void set_nsd(struct bn *x)
	{
	x->nsd = vec_nsd(x->len,x->vec);
	}

// Return x+y where x->nsd >= y->nsd, ignoring sign.
static struct bn *nat_add(const struct bn *x, const struct bn *y)
	{
	struct bn *z = bn_new(x->nsd+1);
	vec_add(x->nsd,x->vec,y->nsd,y->vec,z->vec);
	set_nsd(z);
	return z;
	}

// Return x-y where x >= y, ignoring sign.
static struct bn *nat_sub(const struct bn *x, const struct bn *y)
	{
	struct bn *z = bn_new(x->nsd);
	vec_sub(x->nsd,x->vec,y->nsd,y->vec,z->vec);
	set_nsd(z);
	return z;
	}

static struct bn *bn_add_sub(int add, const struct bn *x, const struct bn *y)
	{
	if (add)
		{
		struct bn *z = (x->nsd >= y->nsd) ? nat_add(x,y) : nat_add(y,x);
		z->sign = x->sign;
		return z;
		}
	else
		{
		int cmp = nat_cmp(x,y);
		struct bn *z = (cmp >= 0) ? nat_sub(x,y) : nat_sub(y,x);
		z->sign = (cmp == (x->sign ? 1 : -1));
		return z;
		}
	}

// Return z = x+y.
struct bn *bn_add(const struct bn *x, const struct bn *y)
	{
	return bn_add_sub((x->sign == y->sign), x, y);
	}

// Return z = x-y.
struct bn *bn_sub(const struct bn *x, const struct bn *y)
	{
	return bn_add_sub((x->sign != y->sign), x, y);
	}

// Return z = x*y.
struct bn *bn_mul(const struct bn *x, const struct bn *y)
	{
	u64 xn = x->nsd;
	u64 yn = y->nsd;
	struct bn *z = bn_new(xn + yn);

	vec_mul(xn,x->vec,yn,y->vec,z->vec);
	set_nsd(z);
	z->sign = (x->sign == y->sign) ? 0 : (z->nsd != 0);
	return z;
	}

// Construct an integer from a string of decimal digits.
//
// The string may start with a leading '-' or '+'.  After that, a NUL or any
// other non-digit is treated as the end of the string.
//
// The maximum number of decimal digits is 38,654,705,654, which ensures that I
// can represent the result.  The routine dies if that limit is exceeded.
//
// NOTE:  I estimate the size of the result in advance as follows.
//
// Let B = 2^32, the base I use for numbers.
// Let len = strlen(s).  Note that len >= 0.
// The maximum possible number is M = 10^len-1, assuming all digits are '9'.
// Let xn = 1 + floor(len/9).
// It is now guaranteed that M <= B^xn-1, so M fits in the result.
//
// Proof by contradiction:
//
// Define logB(x) = ln(x)/ln(B).
// Define q = logB(10).  Note that q < 1/9.
//
// : M > B^xn - 1  # Assume the contrary.
// : 10^len - 1 > B^xn - 1
// : 10^len > B^xn
// : (B^q)^len > B^xn
// : B^(q*len) > B^xn
// : q*len > xn
// : q*len > 1 + floor(len/9)
// : q*len > 1 + (len/9 - 1)
// : q*len > (1/9)*len
// : (len=0 and 0>0) or (len>0 and q>1/9)
// : (len=0 and false) or (len>0 and false)
// : false or false
// : false
// QED

struct bn *bn_from_dec(const char *s)
	{
	u32 sign = 0;
	if (*s == '-')
		{
		sign = 1;
		s++;
		}
	else if (*s == '+')
		s++;

	{
	const u64 len = strlen(s);
	const u64 xn = 1 + (len/9);
	struct bn *x = bn_new(xn);

	vec_from_dec(s,xn,x->vec);
	set_nsd(x);
	x->sign = sign ? (x->nsd != 0) : 0;
	return x;
	}
	}

void bn_div
	(
	const struct bn *x,
	const struct bn *y,
	struct bn **qp,
	struct bn **rp
	)
	{
	struct bn *q = bn_new(x->nsd);
	struct bn *r = bn_new(x->nsd);

	vec_div(x->nsd,x->vec,y->nsd,y->vec,q->vec,r->vec);
	set_nsd(q);
	set_nsd(r);

	q->sign = x->sign ^ y->sign;
	r->sign = x->sign;

	*qp = q;
	*rp = r;
	}

struct bn *bn_mod(const struct bn *x, const struct bn *y)
	{
	struct bn *r = bn_new(x->nsd);
	vec_mod(x->nsd,x->vec,y->nsd,y->vec,r->vec);
	set_nsd(r);
	r->sign = x->sign;
	return r;
	}

// Buffer the decimal digits in a base B number.
static void vec_to_dec(buffer buf, u32 xn, const u32 *x, u32 *q, u32 *r)
	{
	u32 y[1] = {10};

	if (xn == 0)
		buf_add(buf,'0');

	while (xn)
		{
		char ch;
		vec_div(xn,x,1,y,q,r);

		ch = r[0] + '0';
		buf_add(buf,ch);
		x = q;
		xn = vec_nsd(xn,x);
		}
	}

// Reverse a string in place.
static void reverse_string(unsigned long len, char *s)
	{
	unsigned long mid = len / 2;
	unsigned long i;

	for (i = 0; i < mid; i++)
		{
		char ch = s[len-1-i];
		s[len-1-i] = s[i];
		s[i] = ch;
		}
	}

string bn_to_dec(const struct bn *x)
	{
	struct bn *q = bn_new(x->nsd);
	struct bn *r = bn_new(x->nsd);
	struct buffer buf = {0};
	string str;

	vec_to_dec(&buf,x->nsd,x->vec,q->vec,r->vec);
	if (x->sign)
		buf_add(&buf,'-');

	bn_free(q);
	bn_free(r);

	str = buf_clear(&buf);
	reverse_string(str->len,str->data);
	return str;
	}
