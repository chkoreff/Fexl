#include <format.h>
#include <string.h> /* strlen */
#include <output.h>
#include <unistd.h> /* write */

/* Put data to current output. */
void putd(int out, const char *data, unsigned long len)
	{
	ssize_t n = write(out,data,len);
	(void)n;
	}

void put(int out, const char *data)
	{
	putd(out, data, strlen(data));
	}

void put_ch(int out, char ch)
	{
	char buf[1];
	buf[0] = ch;
	putd(out,buf,1);
	}

void put_long(int out, long x)
	{
	put(out,format_long(x));
	}

void put_ulong(int out, unsigned long x)
	{
	put(out,format_ulong(x));
	}

void put_double(int out, double x)
	{
	put(out,format_double(x));
	}

void nl(int out)
	{
	putd(out,"\n",1);
	}
