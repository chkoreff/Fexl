#include <format.h>
#include <string.h> /* strlen */
#include <output.h>
#include <unistd.h> /* write */

/* Initially we put data to stdout. */
int cur_out = STDOUT_FILENO;

/* Put data to current output. */
void putd(const char *data, unsigned long len)
	{
	ssize_t n = write(cur_out,data,len);
	(void)n;
	}

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

static void set_output(int fd)
	{
	fsync(cur_out);
	cur_out = fd;
	}

void put_to_error(void)
	{
	set_output(STDERR_FILENO);
	}

void put_to_output(void)
	{
	set_output(STDOUT_FILENO);
	}
