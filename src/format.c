#include <stdio.h>

#include <format.h>

static char buf[100]; // Being careful here.

const char *format_double(double x)
	{
	snprintf(buf, sizeof(buf), "%.15g", x);
	return buf;
	}
