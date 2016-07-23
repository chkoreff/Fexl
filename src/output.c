#include <format.h>
#include <stdio.h>
#include <string.h> /* strlen */
#include <output.h>

void *cur_out;

/* Put data to current output file. */
static void putd_file(const char *data, unsigned long len)
	{
	size_t n = fwrite(data,1,len,cur_out);
	(void)n;
	}

output putd;

void put(const char *data)
	{
	putd(data,strlen(data));
	}

void put_ch(char ch)
	{
	putd(&ch,1);
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

void put_to(void *out)
	{
	putd = putd_file;
	cur_out = out;
	}

void put_to_error(void)
	{
	put_to(stderr);
	}

/* Force a write of all buffered data to the current output. */
void flush(void)
	{
	/* Only call fflush if current output is a file. */
	if (putd == putd_file)
		fflush(cur_out);
	}
