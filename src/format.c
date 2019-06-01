#include <stdint.h>

#include <format.h>
#include <stdio.h> /* snprintf */

static char buf[100]; /* Being careful here. */

const char *format_long(long x)
	{
	snprintf(buf, sizeof(buf), "%ld", x);
	return buf;
	}

const char *format_ulong(unsigned long x)
	{
	snprintf(buf, sizeof(buf), "%lu", x);
	return buf;
	}

const char *format_uint64_t(uint64_t x)
	{
	snprintf(buf, sizeof(buf), "%lu", x);
	return buf;
	}

/*
I show 15 digits because that's what Perl does.  I considered using limits.h
to determine a machine-specific precision.  The problem is, DECIMAL_DIG (in
float.h) is only guaranteed to be at least 10, which is a pretty poor worst
case.

https://en.wikipedia.org/wiki/Double-precision_floating-point_format
*/
const char *format_double(double x)
	{
	snprintf(buf, sizeof(buf), "%.15g", x);
	return buf;
	}
