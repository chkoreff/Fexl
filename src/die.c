#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "die.h"

void warn(const char *format, ...)
	{
	fflush(stdout);

	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fputc('\n', stderr);
	}

void die(const char *format, ...)
	{
	warn(format);
	exit(1);
	}
