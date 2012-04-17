#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "die.h"
#include "run.h"

void warn(const char *format, ...)
	{
	fflush(stdout);

	if (main_argv)
		{
		const char *script_name = (0 < argi && argi < main_argc)
			? main_argv[argi] : "";
		fprintf(stderr, "%s:%s:%ld: ", main_argv[0], script_name, line);
		}

	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	if (errno) fprintf(stderr, ": %s", strerror(errno));
	fputc('\n', stderr);
	}

void die(const char *format, ...)
	{
	warn(format);
	exit(1);
	}
