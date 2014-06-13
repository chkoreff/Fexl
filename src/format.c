#include <num.h>

#include <format.h>
#include <stddef.h> /* size_t */

/* NOTE: On some machines, when compiling in -ansi mode, I can't simply include
stdio.h to get the definition of snprintf, so instead I do my own declaration
here. */
extern int snprintf(char *str, size_t size, const char *format, ...);

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

/* We show 15 digits because that's what Perl does. */

/* https://en.wikipedia.org/wiki/Double-precision_floating-point_format */
/* LATER use limits to determine precision for machine */
/* LATER problem is, DECIMAL_DIG (in float.h) is only guaranteed to be at least
10. */
const char *format_double(double x)
	{
	snprintf(buf, sizeof(buf), "%.15g", x);
	return buf;
	}
