#include <stdint.h>

#include <format.h>
#include <stdio.h> // snprintf

static char buf[100]; // Being careful here.

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
I show 15 digits because that's what Perl does, and it seems to work well.

I could instead use DBL_DECIMAL_DIG from float.h, like this:

	snprintf(buf, sizeof(buf), "%.*g", DBL_DECIMAL_DIG, x);

Now DBL_DECIMAL_DIG is only guaranteed to be at least 10, which is a pretty low
worst case.  In practice the value is commonly 17, but I think that shows too
many digits when I convert from a string using strtod (see convert.c).

For example, if I convert "5.8" to a double and then show it with 17 digits, it
shows as "5.7999999999999998", but with 15 digits is shows as "5.8".

Using DBL_DECIMAL_DIG guarantees that every double will survive a round-trip
conversion unchanged, which is a significant advantage.  So I would like to use
it here, but I'm still torn on what to do with a case like "5.8" above.  Maybe
I'll use a separate "stream_double" call when I really need it.

https://en.wikipedia.org/wiki/Double-precision_floating-point_format
https://en.cppreference.com/c/types/limits
*/
const char *format_double(double x)
	{
	snprintf(buf, sizeof(buf), "%.15g", x);
	return buf;
	}
