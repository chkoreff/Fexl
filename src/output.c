#include <stdio.h>
#include <string.h>
#include <output.h>

static void put_fh(FILE *fh, const char *data, unsigned long len)
	{
	size_t count = fwrite(data, 1, len, fh);
	(void)count;  /* Ignore the return count. */
	}

/* Put data to stdout. */
static void target_out(const char *data, unsigned long len)
	{
	put_fh(stdout,data,len);
	}

/* Put data to stderr. */
static void target_err(const char *data, unsigned long len)
	{
	put_fh(stderr,data,len);
	}

/* Put data to the current target. */
target putd = target_out;

target beg_error(void)
	{
	target old = putd;
	putd = target_err;
	return old;
	}

void end_error(target old)
	{
	putd = old;
	}

void put(const char *data)
	{
	putd(data, strlen(data));
	}

void nl(void)
	{
	putd("\n",1);
	}

void put_long(long x)
	{
	char buf[100]; /* Being careful here. */
	snprintf(buf, sizeof(buf), "%ld", x);
	put(buf);
	}

void put_ulong(unsigned long x)
	{
	char buf[100]; /* Being careful here. */
	snprintf(buf, sizeof(buf), "%lu", x);
	put(buf);
	}

/* We show 15 digits because that's what Perl does. */

/* https://en.wikipedia.org/wiki/Double-precision_floating-point_format */
/* LATER use limits to determine precision for machine */
/* LATER problem is, DECIMAL_DIG (in float.h) is only guaranteed to be at least
10. */
void put_double(double x)
	{
	char buf[100]; /* Being careful here. */
	snprintf(buf, sizeof(buf), "%.15g", x);
	put(buf);
	}
