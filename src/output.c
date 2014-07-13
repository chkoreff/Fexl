#include <format.h>
#include <stdio.h>
#include <str.h>
#include <string.h>
#include <output.h>

static void put_fh(FILE *fh, const char *data, unsigned long len)
	{
	size_t count = fwrite(data, 1, len, fh);
	(void)count;  /* Ignore the return count. */
	}

/* Put data to stdout. */
static void putd_out(const char *data, unsigned long len)
	{
	put_fh(stdout,data,len);
	}

/* Put data to stderr. */
static void putd_err(const char *data, unsigned long len)
	{
	put_fh(stderr,data,len);
	}

/* Initially we put data to stdout. */
output putd = putd_out;

void put(const char *data)
	{
	putd(data, strlen(data));
	}

void put_ch(char ch)
	{
	char buf[1];
	buf[0] = ch;
	putd(buf,1);
	}

void put_long(long x)
	{
	put(format_long(x));
	}

void put_ulong(unsigned long x)
	{
	put(format_ulong(x));
	}

void put_double(double x)
	{
	put(format_double(x));
	}

void nl(void)
	{
	putd("\n",1);
	}

void put_to_error(void)
	{
	fflush(stdout);
	putd = putd_err;
	}
