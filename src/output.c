#include <stdio.h>

#include <file.h>
#include <output.h>

void put(const char *data)
	{
	fput(stdout,data);
	}

void put_ch(char ch)
	{
	fput_ch(stdout,ch);
	}

void put_long(long x)
	{
	fput_long(stdout,x);
	}

void put_ulong(unsigned long x)
	{
	fput_ulong(stdout,x);
	}

void put_double(double x)
	{
	fput_double(stdout,x);
	}

void nl(void)
	{
	fnl(stdout);
	}
