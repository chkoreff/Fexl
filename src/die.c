void warn(const char *format, ...)
	{
	fflush(stdout);

	if (format == 0) return;
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
